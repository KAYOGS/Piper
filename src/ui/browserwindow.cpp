#include "browserwindow.h"
#include <QDir>
#include <QIcon>
#include <QFileDialog>
#include <QPainter>
#include <QWebEngineSettings>
#include <QFileInfo>
#include <QAction>
#include <QScrollBar>

HomeView::HomeView(QWidget *parent) : QWidget(parent) { this->setObjectName("HomeView"); }
void HomeView::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QSettings s("PiperOrg", "PiperBrowser");
    QString bg = s.value("background").toString();
    if(!bg.isEmpty() && QFile::exists(bg)) p.drawPixmap(rect(), QPixmap(bg).scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    else p.fillRect(rect(), QColor("#121212"));
}

BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent), m_isPrivate(false)
{
    if (!profile) profile = QWebEngineProfile::defaultProfile();
    setWindowIcon(QIcon("res/icons/piperos.png"));
    setWindowTitle("Piper Browser");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    topBar = new QWidget();
    topBar->setFixedHeight(45);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    
    auto styleBtn = [](QPushButton* b, QString t, QString tip) {
        b->setText(t); b->setToolTip(tip); b->setFixedSize(34, 34); b->setCursor(Qt::PointingHandCursor);
    };

    backButton = new QPushButton(); styleBtn(backButton, "←", "Voltar");
    refreshButton = new QPushButton(); styleBtn(refreshButton, "↻", "Atualizar");
    homeButton = new QPushButton(); styleBtn(homeButton, "🏠", "Home");
    urlEdit = new QLineEdit(); urlEdit->setFixedHeight(30);
    favAddButton = new QPushButton(); styleBtn(favAddButton, "⭐", "Favoritar");
    tabsButton = new QPushButton("0"); styleBtn(tabsButton, "0", "Abas");
    addTabButton = new QPushButton(); styleBtn(addTabButton, "+", "Nova Aba");
    menuButton = new QPushButton(); styleBtn(menuButton, "☰", "Menu");

    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(urlEdit, 1);
    topLayout->addWidget(favAddButton);
    topLayout->addWidget(tabsButton);
    topLayout->addWidget(addTabButton);
    topLayout->addWidget(menuButton);

    stackedWidget = new QStackedWidget();
    rootLayout->addWidget(topBar);
    rootLayout->addWidget(stackedWidget);

    tabSwitcherWidget = new QWidget(centralWidget);
    tabSwitcherWidget->hide();
    QVBoxLayout *swLayout = new QVBoxLayout(tabSwitcherWidget);
    
    tabScrollArea = new QScrollArea();
    tabScrollArea->setWidgetResizable(true);
    tabScrollArea->setFrameShape(QFrame::NoFrame);
    tabGridContainer = new QWidget();
    tabGridLayout = new QGridLayout(tabGridContainer);
    tabScrollArea->setWidget(tabGridContainer);
    
    QPushButton *clearTabsBtn = new QPushButton("🗑️ Limpar Todas as Abas");
    clearTabsBtn->setFixedHeight(35);
    clearTabsBtn->setStyleSheet("background: #E74C3C; color: white; border-radius: 5px; font-weight: bold; margin-bottom: 5px;");
    
    QPushButton *closeSw = new QPushButton("Voltar para o Navegador");
    closeSw->setFixedHeight(40);
    closeSw->setStyleSheet("background: #4169E1; color: white; border-radius: 5px;");

    swLayout->addWidget(tabScrollArea);
    swLayout->addWidget(clearTabsBtn);
    swLayout->addWidget(closeSw);

    connect(urlEdit, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlEntered);
    connect(addTabButton, &QPushButton::clicked, [this](){ createNewTab(); });
    connect(tabsButton, &QPushButton::clicked, this, &BrowserWindow::toggleTabSwitcher);
    connect(closeSw, &QPushButton::clicked, this, &BrowserWindow::toggleTabSwitcher);
    connect(clearTabsBtn, &QPushButton::clicked, this, &BrowserWindow::clearAllTabs);
    
    connect(backButton, &QPushButton::clicked, [this](){ 
        if(auto *v = qobject_cast<QWebEngineView*>(stackedWidget->currentWidget())) v->back(); 
    });
    connect(refreshButton, &QPushButton::clicked, [this](){ 
        if(auto *v = qobject_cast<QWebEngineView*>(stackedWidget->currentWidget())) v->reload(); 
    });
    
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(favAddButton, &QPushButton::clicked, this, &BrowserWindow::addFavorite);
    connect(menuButton, &QPushButton::clicked, this, &BrowserWindow::showMainMenu);
    connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    loadSettings();
    applyDarkTheme();
    
    QTimer::singleShot(150, this, [this](){
        createNewTab();
        showMaximized();
    });
}

void BrowserWindow::createNewTab(const QUrl &url) {
    QWidget *view;
    if (url.isEmpty()) {
        view = new HomeView();
        urlEdit->clear();
        urlEdit->setFocus();
    } else {
        QWebEngineView *web = new QWebEngineView();
        setupWebView(web);
        web->load(url);
        view = web;
    }

    TabInfo info;
    info.widget = view;
    info.title = url.isEmpty() ? "Início" : "Carregando...";
    m_tabs.append(info);

    stackedWidget->addWidget(view);
    stackedWidget->setCurrentWidget(view);
    tabsButton->setText(QString::number(m_tabs.size()));
}

void BrowserWindow::setupWebView(QWebEngineView* view) {
    connect(view, &QWebEngineView::titleChanged, [this, view](const QString &t){
        for(auto &tab : m_tabs) { if(tab.widget == view) { tab.title = t; break; } }
    });
    connect(view, &QWebEngineView::urlChanged, [this, view](const QUrl &u){
        if(stackedWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if(!m_isPrivate) {
            historyList.append(u.toString());
            if(historyList.size() > 500) historyList.removeFirst();
            saveSettings();
        }
    });
}

void BrowserWindow::toggleTabSwitcher() {
    if (tabSwitcherWidget->isHidden()) {
        updateCurrentThumbnail();
        refreshTabGrid();
        tabSwitcherWidget->setGeometry(0, topBar->height(), centralWidget->width(), centralWidget->height() - topBar->height());
        tabSwitcherWidget->show();
        tabSwitcherWidget->raise();
    } else {
        tabSwitcherWidget->hide();
    }
}

void BrowserWindow::updateCurrentThumbnail() {
    int idx = stackedWidget->currentIndex();
    if (idx >= 0 && idx < m_tabs.size()) {
        m_tabs[idx].thumbnail = stackedWidget->currentWidget()->grab();
    }
}

void BrowserWindow::refreshTabGrid() {
    QLayoutItem *item;
    while ((item = tabGridLayout->takeAt(0))) {
        if(item->widget()) item->widget()->deleteLater();
        delete item;
    }

    for (int i = 0; i < m_tabs.size(); ++i) {
        QWidget *card = new QWidget();
        card->setFixedSize(280, 200); // Aumentado conforme solicitado
        card->setStyleSheet("background: #252525; border-radius: 10px; border: 1px solid #333;");
        QVBoxLayout *l = new QVBoxLayout(card);
        
        QLabel *img = new QLabel();
        img->setFixedSize(260, 140);
        img->setStyleSheet("background: #000;");
        if(!m_tabs[i].thumbnail.isNull()) 
            img->setPixmap(m_tabs[i].thumbnail.scaled(260, 140, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        
        QPushButton *btn = new QPushButton(m_tabs[i].title.left(30));
        btn->setStyleSheet("color: white; border: none; text-align: left; font-weight: bold; padding: 5px;");
        btn->setCursor(Qt::PointingHandCursor);

        l->addWidget(img);
        l->addWidget(btn);

        // Correção da troca de aba (capturando i por valor)
        connect(btn, &QPushButton::clicked, [this, i](){
            stackedWidget->setCurrentIndex(i);
            updateUrlBar(i);
            toggleTabSwitcher();
        });

        tabGridLayout->addWidget(card, i / 3, i % 3);
    }
}

void BrowserWindow::updateUrlBar(int index) {
    if(index < 0 || index >= m_tabs.size()) return;
    if(auto *v = qobject_cast<QWebEngineView*>(m_tabs[index].widget)) {
        urlEdit->setText(v->url().toString());
    } else {
        urlEdit->clear();
        urlEdit->setFocus();
    }
}

void BrowserWindow::clearAllTabs() {
    while(m_tabs.size() > 0) {
        QWidget *w = m_tabs.takeFirst().widget;
        stackedWidget->removeWidget(w);
        w->deleteLater();
    }
    createNewTab();
    toggleTabSwitcher();
}

void BrowserWindow::handleUrlEntered() {
    QString input = urlEdit->text().trimmed();
    if(input.isEmpty()) return;

    QUrl url;
    // Lógica de Busca Inteligente (DuckDuckGo)
    if (input.contains(".") && !input.contains(" ")) {
        url = QUrl::fromUserInput(input);
    } else {
        url = QUrl("https://duckduckgo.com/?q=" + input);
    }

    if (auto *v = qobject_cast<QWebEngineView*>(stackedWidget->currentWidget())) {
        v->load(url);
    } else {
        int idx = stackedWidget->currentIndex();
        QWidget *old = m_tabs[idx].widget;
        
        QWebEngineView *web = new QWebEngineView();
        setupWebView(web);
        web->load(url);
        
        m_tabs[idx].widget = web;
        stackedWidget->insertWidget(idx, web);
        stackedWidget->setCurrentWidget(web);
        old->deleteLater();
    }
}

void BrowserWindow::addFavorite() {
    if(auto v = qobject_cast<QWebEngineView*>(stackedWidget->currentWidget())) {
        QString f = v->title() + "|" + v->url().toString();
        if(!favoritesList.contains(f)) {
            favoritesList.append(f);
            saveSettings();
            
            // Feedback visual de sucesso
            favAddButton->setText("✅");
            favAddButton->setStyleSheet("color: #F1C40F; border: none; font-size: 19px;");
            QTimer::singleShot(2000, this, [this](){
                updateIconsStyle();
                favAddButton->setText("⭐");
            });
        }
    }
}

void BrowserWindow::goHome() {
    int idx = stackedWidget->currentIndex();
    QWidget *old = m_tabs[idx].widget;
    HomeView *home = new HomeView();
    m_tabs[idx].widget = home;
    m_tabs[idx].title = "Início";
    stackedWidget->insertWidget(idx, home);
    stackedWidget->setCurrentWidget(home);
    old->deleteLater();
    urlEdit->clear();
    urlEdit->setFocus();
}

void BrowserWindow::applyDarkTheme() {
    this->setStyleSheet("QMainWindow { background: #121212; }");
    topBar->setStyleSheet("background: #1f1f1f; border-bottom: 1px solid #333;");
    urlEdit->setStyleSheet("background: #2c2c2c; color: white; border: 1px solid #444; border-radius: 8px; padding: 0 12px;");
    tabSwitcherWidget->setStyleSheet("background: rgba(15, 15, 15, 252);");
    updateIconsStyle();
}

void BrowserWindow::updateIconsStyle() {
    QString style = "QPushButton { color: white; border: none; font-size: 19px; border-radius: 8px; background: transparent; } "
                    "QPushButton:hover { background: rgba(128,128,128,0.2); }";
    backButton->setStyleSheet(style); refreshButton->setStyleSheet(style);
    homeButton->setStyleSheet(style); favAddButton->setStyleSheet(style); menuButton->setStyleSheet(style);
    tabsButton->setStyleSheet("color: #4169E1; font-weight: bold; border: 1px solid #4169E1; border-radius: 4px;");
    addTabButton->setStyleSheet("background: #333; color: white; font-weight: bold; border-radius: 4px;");
}

void BrowserWindow::showMainMenu() {
    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #1f1f1f; color: white; border: 1px solid #333; } QMenu::item:selected { background-color: #4169E1; }");
    connect(menu.addAction("🔖 Favoritos"), &QAction::triggered, this, &BrowserWindow::showFavoritesMenu);
    connect(menu.addAction("📜 Histórico"), &QAction::triggered, this, &BrowserWindow::showHistory);
    connect(menu.addAction("📥 Downloads"), &QAction::triggered, this, &BrowserWindow::showDownloads);
    connect(menu.addAction("🕵 Janela Privada"), &QAction::triggered, this, &BrowserWindow::openPrivateWindow);
    connect(menu.addAction("🖼️ Wallpaper"), &QAction::triggered, this, &BrowserWindow::changeBackgroundImage);
    menu.addSeparator();
    connect(menu.addAction("👤 Sobre"), &QAction::triggered, this, &BrowserWindow::showAbout);
    menu.exec(menuButton->mapToGlobal(QPoint(0, menuButton->height())));
}
void BrowserWindow::loadSettings() {
    QSettings s("PiperOrg", "PiperBrowser");
    currentBgPath = s.value("background", "").toString();
    favoritesList = s.value("favorites").toStringList();
    historyList = s.value("history").toStringList();
}
void BrowserWindow::saveSettings() {
    QSettings s("PiperOrg", "PiperBrowser");
    s.setValue("background", currentBgPath);
    s.setValue("favorites", favoritesList);
    s.setValue("history", historyList);
}
void BrowserWindow::resizeEvent(QResizeEvent *e) { QMainWindow::resizeEvent(e); if(!tabSwitcherWidget->isHidden()) tabSwitcherWidget->setGeometry(0, topBar->height(), centralWidget->width(), centralWidget->height() - topBar->height()); }
void BrowserWindow::changeBackgroundImage() {
    QString p = QFileDialog::getOpenFileName(this, "Wallpaper", "", "Imagens (*.png *.jpg)");
    if(!p.isEmpty()) { currentBgPath = p; saveSettings(); update(); }
}
void BrowserWindow::showFavoritesMenu() {
    QMenu m(this); for(const QString &f : favoritesList) m.addAction(f.split("|").first(), [this, f](){ createNewTab(QUrl(f.split("|").last())); });
    m.exec(QCursor::pos());
}
void BrowserWindow::handleDownload(QWebEngineDownloadRequest *d) { d->accept(); m_downloads.append(d); }
void BrowserWindow::showDownloads() { QDialog d(this); d.setWindowTitle("Downloads"); d.resize(300, 400); d.exec(); }
void BrowserWindow::showHistory() {
    QDialog d(this); d.setWindowTitle("Histórico"); d.resize(400, 500); QVBoxLayout *l = new QVBoxLayout(&d); QListWidget *lw = new QListWidget();
    for(int i=historyList.size()-1; i>=0; --i) lw->addItem(historyList.at(i));
    l->addWidget(lw); d.exec();
}
void BrowserWindow::showAbout() { QDialog d(this); d.setFixedSize(300, 150); QVBoxLayout *l = new QVBoxLayout(&d); l->addWidget(new QLabel("Piper Browser\nPIX: a2740c20-8084-4ca6-af32-18aaa0e86892")); d.exec(); }
void BrowserWindow::openPrivateWindow() { BrowserWindow *pw = new BrowserWindow(new QWebEngineProfile(this)); pw->m_isPrivate = true; pw->show(); }
void BrowserWindow::checkTabActivity() {}
BrowserWindow::~BrowserWindow() {}
