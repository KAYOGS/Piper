#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QDir>
#include <QIcon>
#include <QTimer>
#include <QFileDialog>
#include <QPainter>
#include <QWebEngineSettings>
#include <QSettings>
#include <QMenu>
#include <QTabBar>
#include <QFileInfo>
#include <QWebChannel>
#include <QAction>

// --- HOMEVIEW ---
HomeView::HomeView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    QLabel *logo = new QLabel("PIPER");
    logo->setStyleSheet("font-size: 50px; font-weight: bold; color: #4169E1; letter-spacing: 15px; background: transparent;");
    QLabel *sub = new QLabel("Navegação Veloz para Hardware Modesto");
    sub->setStyleSheet("font-size: 20px; color: #4169E1; letter-spacing: 3px; font-weight: 300; background: transparent;");
    layout->addStretch();
    layout->addWidget(logo, 0, Qt::AlignCenter);
    layout->addWidget(sub, 0, Qt::AlignCenter);
    layout->addStretch();
}

void HomeView::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QSettings s("PiperOrg", "PiperBrowser");
    QString bg = s.value("background").toString();
    if(!bg.isEmpty() && QFile::exists(bg)) {
        p.drawPixmap(rect(), QPixmap(bg).scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else { p.fillRect(rect(), QColor("#121212")); }
}

// --- BROWSERWINDOW ---
BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent), m_isPrivate(false)
{
    if (!profile) profile = QWebEngineProfile::defaultProfile();
    setWindowIcon(QIcon("res/icons/piperos.png"));
    setWindowTitle("Piper Browser");
    showMaximized();

    profile->setUrlRequestInterceptor(new UrlInterceptor(this));
    m_channel = new QWebChannel(this);
    m_channel->registerObject(QStringLiteral("piper"), this);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    topBar = new QWidget();
    topBar->setFixedHeight(45);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);

    auto styleBtn = [](QPushButton* b, QString t, QString tip) {
        b->setText(t); b->setToolTip(tip); b->setFixedSize(34, 34); b->setCursor(Qt::PointingHandCursor);
    };

    backButton = new QPushButton(); styleBtn(backButton, "←", "Voltar");
    refreshButton = new QPushButton(); styleBtn(refreshButton, "↻", "Atualizar");
    homeButton = new QPushButton(); styleBtn(homeButton, "🏠", "Home");
    favAddButton = new QPushButton(); styleBtn(favAddButton, "⭐", "Favoritar");
    menuButton = new QPushButton(); styleBtn(menuButton, "☰", "Menu");

    urlEdit = new QLineEdit(); 
    urlEdit->setFixedHeight(30);
    urlEdit->setPlaceholderText("Pesquise ou digite a URL...");

    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(urlEdit, 1);
    topLayout->addWidget(favAddButton);
    topLayout->addWidget(menuButton);

    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    
    // ATIVAÇÃO DO SCROLL LATERAL NAS ABAS
    tabWidget->setUsesScrollButtons(true);
    tabWidget->tabBar()->setExpanding(false);
    tabWidget->tabBar()->setElideMode(Qt::ElideRight);

    addTabButton = new QPushButton("+", tabWidget);
    addTabButton->setFixedSize(22, 22);
    addTabButton->setCursor(Qt::PointingHandCursor);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);

    m_activityTimer = new QTimer(this);
    connect(m_activityTimer, &QTimer::timeout, this, &BrowserWindow::checkTabActivity);
    m_activityTimer->start(30000); 

    connect(urlEdit, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlEntered);
    connect(addTabButton, &QPushButton::clicked, [this](){ createNewTab(); });
    connect(backButton, &QPushButton::clicked, [this](){ if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back(); });
    connect(refreshButton, &QPushButton::clicked, [this](){ if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload(); });
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(favAddButton, &QPushButton::clicked, this, &BrowserWindow::addFavorite);
    connect(menuButton, &QPushButton::clicked, this, &BrowserWindow::showMainMenu);
    connect(tabWidget, &QTabWidget::currentChanged, this, &BrowserWindow::onTabChanged);
    connect(tabWidget, &QTabWidget::tabCloseRequested, [this](int i){
        QWidget *w = tabWidget->widget(i);
        m_tabLastActivity.remove(w);
        tabWidget->removeTab(i);
        w->deleteLater();
        if(tabWidget->count() == 0) createNewTab();
        syncTabButtonPos();
    });
    
    connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    loadSettings();
    applyDarkTheme();
    createNewTab();
}

void BrowserWindow::applyDarkTheme() {
    this->setStyleSheet("QMainWindow { background: #121212; }");
    topBar->setStyleSheet("background: #1f1f1f; border-bottom: 1px solid #333;");
    urlEdit->setStyleSheet("background: #2c2c2c; color: white; border: 1px solid #444; border-radius: 8px; padding: 0 12px;");
    
    // CSS REFORMULADO PARA ABAS COM SUPORTE A SCROLL E ICONES
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; } "
        "QTabBar::tab { background: #1f1f1f; min-width: 140px; max-width: 180px; color: #888; padding: 6px 10px; border-right: 1px solid #333; font-size: 12px; } "
        "QTabBar::tab:selected { background: #121212; color: white; border-bottom: 2px solid #4169E1; } "
        "QTabBar::close-button { image: url(res/icons/close.png); subcontrol-position: right; background: rgba(255,0,0,0.2); border-radius: 2px;} "
    );
    updateIconsStyle();
}

void BrowserWindow::updateIconsStyle() {
    QString style = "QPushButton { color: white; border: none; font-size: 19px; border-radius: 8px; background: transparent; } "
                    "QPushButton:hover { background: rgba(128,128,128,0.2); }";
    backButton->setStyleSheet(style); refreshButton->setStyleSheet(style);
    homeButton->setStyleSheet(style); favAddButton->setStyleSheet(style); menuButton->setStyleSheet(style);
    addTabButton->setStyleSheet("QPushButton { color: white; border: none; font-size: 18px; font-weight: bold; border-radius: 6px; background: #444; } QPushButton:hover { background: #111; }");
}

void BrowserWindow::handleUrlEntered() {
    QString input = urlEdit->text().trimmed();
    if(input.isEmpty()) return;
    
    QUrl url = (input.contains(" ") || (!input.contains(".") && !input.contains("://"))) 
               ? QUrl("https://www.google.com/search?q=" + input)
               : QUrl::fromUserInput(input.contains("://") ? input : "https://" + input);

    int currentIndex = tabWidget->currentIndex();
    QWidget *currentWidget = tabWidget->currentWidget();

    // BUG FIX: Se estiver na HomeView, transforma a aba atual em WebView em vez de abrir nova
    if (qobject_cast<HomeView*>(currentWidget)) {
        QWebEngineView *view = new QWebEngineView();
        setupWebView(view);
        tabWidget->removeTab(currentIndex);
        currentIndex = tabWidget->insertTab(currentIndex, view, QIcon("res/icons/piperos.png"), "Carregando...");
        tabWidget->setCurrentIndex(currentIndex);
        currentWidget->deleteLater(); 
        view->load(url);
    } else if (auto *v = qobject_cast<QWebEngineView*>(currentWidget)) {
        v->load(url);
    }
}

void BrowserWindow::setupWebView(QWebEngineView* view) {
    m_tabLastActivity[view] = QDateTime::currentDateTime();
    
    // ATUALIZAÇÃO DINÂMICA DE TÍTULO E FAVICON
    connect(view, &QWebEngineView::titleChanged, [this, view](const QString &t){
        int i = tabWidget->indexOf(view);
        if(i != -1) tabWidget->setTabText(i, t.isEmpty() ? "Nova Aba" : t);
    });
    
    connect(view, &QWebEngineView::iconChanged, [this, view](const QIcon &icon){
        int i = tabWidget->indexOf(view);
        if(i != -1) tabWidget->setTabIcon(i, icon.isNull() ? QIcon("res/icons/piperos.png") : icon);
    });

    connect(view, &QWebEngineView::urlChanged, [this, view](const QUrl &u){
        if(tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if(!m_isPrivate) {
            historyList.append(u.toString());
            if(historyList.size() > 1000) historyList.removeFirst();
            saveSettings();
        }
    });
}

void BrowserWindow::createNewTab(const QUrl &url) {
    if (url.isEmpty()) {
        HomeView *home = new HomeView();
        int idx = tabWidget->addTab(home, QIcon("res/icons/piperos.png"), "Início");
        tabWidget->setCurrentIndex(idx);
        urlEdit->clear();
        urlEdit->setFocus();
    } else {
        QWebEngineView *view = new QWebEngineView();
        setupWebView(view);
        int idx = tabWidget->addTab(view, QIcon("res/icons/piperos.png"), "Carregando...");
        view->load(url);
        tabWidget->setCurrentIndex(idx);
    }
    syncTabButtonPos();
}

void BrowserWindow::goHome() {
    int idx = tabWidget->currentIndex();
    if(idx != -1) {
        QWidget *w = tabWidget->widget(idx);
        tabWidget->removeTab(idx);
        w->deleteLater();
        createNewTab(QUrl());
    }
}

void BrowserWindow::onTabChanged(int index) {
    syncTabButtonPos();
    if (index == -1) return;
    QWidget *current = tabWidget->widget(index);
    m_tabLastActivity[current] = QDateTime::currentDateTime();
    if (auto *view = qobject_cast<QWebEngineView*>(current)) {
        urlEdit->setText(view->url().toString());
        view->page()->setLifecycleState(QWebEnginePage::LifecycleState::Active);
    } else { 
        urlEdit->clear(); 
        urlEdit->setFocus();
    }
}

// ... Restante das funções permanecem conforme o padrão consolidado anteriormente ...

void BrowserWindow::showMainMenu() {
    QMenu menu(this);
    menu.setFixedWidth(240);
    menu.setStyleSheet("QMenu { background-color: #1f1f1f; color: white; border: 1px solid #333; padding: 5px; } "
                       "QMenu::item { padding: 10px 25px; border-radius: 4px; } "
                       "QMenu::item:selected { background-color: #4169E1; }");
    connect(menu.addAction("🔖 Ver Favoritos"), &QAction::triggered, this, &BrowserWindow::showFavoritesMenu);
    connect(menu.addAction("📜 Histórico"), &QAction::triggered, this, &BrowserWindow::showHistory);
    connect(menu.addAction("📥 Downloads"), &QAction::triggered, this, &BrowserWindow::showDownloads);
    connect(menu.addAction("🕵 Nova Janela Privada"), &QAction::triggered, this, &BrowserWindow::openPrivateWindow);
    connect(menu.addAction("🖼️ Alterar Papel de Parede"), &QAction::triggered, this, &BrowserWindow::changeBackgroundImage);
    menu.addSeparator();
    connect(menu.addAction("👤 Sobre o Autor"), &QAction::triggered, this, &BrowserWindow::showAbout);
    menu.exec(menuButton->mapToGlobal(QPoint(0, menuButton->height())));
}

void BrowserWindow::syncTabButtonPos() {
    QTimer::singleShot(100, [this](){
        if(tabWidget->count() > 0) {
            QRect r = tabWidget->tabBar()->tabRect(tabWidget->count()-1);
            addTabButton->move(r.right() + 12, r.top() + 5);
            addTabButton->show();
        }
    });
}

void BrowserWindow::resizeEvent(QResizeEvent *e) { QMainWindow::resizeEvent(e); syncTabButtonPos(); }
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
void BrowserWindow::changeBackgroundImage() {
    QString p = QFileDialog::getOpenFileName(this, "Escolher Papel de Parede", "", "Imagens (*.png *.jpg *.jpeg)");
    if(!p.isEmpty()) { currentBgPath = p; saveSettings(); repaint(); }
}
void BrowserWindow::addFavorite() {
    if(auto v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        QString f = v->title() + "|" + v->url().toString();
        if(!favoritesList.contains(f)) { favoritesList.append(f); saveSettings(); }
    }
}
void BrowserWindow::showFavoritesMenu() {
    QMenu m(this);
    m.setStyleSheet("QMenu { background: #1f1f1f; color: white; } QMenu::item:selected { background: #4169E1; }");
    for(const QString &f : favoritesList) {
        QStringList p = f.split("|");
        m.addAction(p.first(), [this, p](){ createNewTab(QUrl(p.last())); });
    }
    m.exec(QCursor::pos());
}
void BrowserWindow::handleDownload(QWebEngineDownloadRequest *d) {
    QString p = QFileDialog::getSaveFileName(this, "Salvar", QDir::homePath() + "/" + d->downloadFileName());
    if(p.isEmpty()) d->cancel(); else { d->setDownloadDirectory(QFileInfo(p).absolutePath()); d->setDownloadFileName(QFileInfo(p).fileName()); d->accept(); m_downloads.append(d); }
}
void BrowserWindow::showDownloads() {
    QMenu m(this);
    m.setStyleSheet("QMenu { background: #1f1f1f; color: white; }");
    for(auto *d : m_downloads) m.addAction(d->downloadFileName() + (d->state()==QWebEngineDownloadRequest::DownloadCompleted ? " (OK)" : " (...)"));
    m.exec(QCursor::pos());
}
void BrowserWindow::showHistory() {
    QDialog d(this); d.setWindowTitle("Histórico"); d.resize(400, 500); d.setStyleSheet("background: #121212; color: white;");
    QVBoxLayout *l = new QVBoxLayout(&d); QListWidget *lw = new QListWidget();
    for(int i=historyList.size()-1; i>=0; --i) lw->addItem(historyList.at(i));
    l->addWidget(lw);
    connect(lw, &QListWidget::itemDoubleClicked, [this, &d](QListWidgetItem *it){ createNewTab(QUrl(it->text())); d.accept(); });
    d.exec();
}
void BrowserWindow::showAbout() {
    QDialog d(this); d.setWindowTitle("Sobre o Autor"); d.setFixedSize(400, 500); d.setStyleSheet("background: white; color: #333;");
    QVBoxLayout *l = new QVBoxLayout(&d);
    QLabel *t = new QLabel("Piper Browser - Projeto 2025"); t->setStyleSheet("font-size: 18px; font-weight: bold; color: #4169E1;");
    QLabel *pTitle = new QLabel("☕ Buy me a coffee"); pTitle->setStyleSheet("color: #E74C3C; font-weight: bold;");
    QLineEdit *pKey = new QLineEdit("a2740c20-8084-4ca6-af32-18aaa0e86892"); pKey->setReadOnly(true);
    QPushButton *c = new QPushButton("Copiar PIX");
    c->setStyleSheet("background: #E74C3C; color: white; border-radius: 5px; height: 40px;");
    connect(c, &QPushButton::clicked, [pKey, c](){ pKey->selectAll(); pKey->copy(); c->setText("Copiado!"); });
    l->addWidget(t, 0, Qt::AlignCenter); l->addSpacing(20); l->addWidget(pTitle, 0, Qt::AlignCenter); l->addWidget(pKey); l->addWidget(c); d.exec();
}
void BrowserWindow::openPrivateWindow() {
    QWebEngineProfile *p = new QWebEngineProfile(this);
    BrowserWindow *pw = new BrowserWindow(p);
    pw->m_isPrivate = true;
    pw->setWindowTitle("Piper (Modo Privado)");
    pw->show();
    pw->urlEdit->setFocus();
}
void BrowserWindow::checkTabActivity() {
    QDateTime now = QDateTime::currentDateTime();
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (i == tabWidget->currentIndex()) continue;
        QWidget *w = tabWidget->widget(i);
        if (auto *view = qobject_cast<QWebEngineView*>(w)) {
            if (view->page()->recentlyAudible()) continue;
            if (m_tabLastActivity[w].secsTo(now) >= CAMADA_3_FROZEN) 
                view->page()->setLifecycleState(QWebEnginePage::LifecycleState::Frozen);
        }
    }
}
BrowserWindow::~BrowserWindow() {}
