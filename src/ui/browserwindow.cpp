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
#include <QDesktopServices>
#include <QStandardPaths>

// --- IMPLEMENTAÇÃO DA DASHBOARD (HOMEVIEW) ---
HomeView::HomeView(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *logo = new QLabel("");
    logo->setStyleSheet("font-size: 85px; font-weight: bold; color: #4169E1; letter-spacing: 15px;");
    
    QLabel *sub = new QLabel("");
    sub->setStyleSheet("font-size: 14px; color: #666; letter-spacing: 3px; font-weight: 300;");

    layout->addStretch();
    layout->addWidget(logo, 0, Qt::AlignCenter);
    layout->addWidget(sub, 0, Qt::AlignCenter);
    layout->addStretch();

    settingsBtn = new QPushButton("⚙️", this);
    settingsBtn->setFixedSize(50, 50);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    settingsBtn->setStyleSheet("background: rgba(255,255,255,0.8); border-radius: 25px; border: 1px solid #ddd; font-size: 22px;");
    
    connect(settingsBtn, &QPushButton::clicked, [this](){
        QMenu m(this);
        m.setStyleSheet("QMenu { background: #fff; border: 1px solid #ccc; } QMenu::item { padding: 8px 25px; } QMenu::item:selected { background: #4169E1; color: white; }");
        m.addAction("Tema Escuro", [this](){ emit changeThemeRequested("dark"); });
        m.addAction("Tema Claro", [this](){ emit changeThemeRequested("light"); });
        m.addAction("Tema Piper (Azul)", [this](){ emit changeThemeRequested("default"); });
        m.addSeparator();
        m.addAction("Trocar Papel de Parede", [this](){ emit changeBgRequested(); });
        m.exec(QCursor::pos());
    });
}

void HomeView::resizeEvent(QResizeEvent *) {
    settingsBtn->move(width() - 70, height() - 70);
}

void HomeView::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QSettings s("PiperOrg", "PiperBrowser");
    QString bg = s.value("background").toString();
    if(!bg.isEmpty() && QFile::exists(bg)) {
        p.drawPixmap(rect(), QPixmap(bg).scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else {
        p.fillRect(rect(), QColor("#f5f7ff"));
    }
}

// --- IMPLEMENTAÇÃO DA JANELA PRINCIPAL (BROWSERWINDOW) ---
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

    // Barra de Ferramentas Superior
    topBar = new QWidget();
    topBar->setFixedHeight(35);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);

    auto styleBtn = [](QPushButton* b, QString t, QString tip) {
        b->setText(t); b->setToolTip(tip); b->setFixedSize(30, 30); b->setCursor(Qt::PointingHandCursor);
    };

    backButton = new QPushButton(); styleBtn(backButton, "←", "Voltar");
    refreshButton = new QPushButton(); styleBtn(refreshButton, "↻", "Atualizar");
    urlEdit = new QLineEdit(); 
    urlEdit->setFixedHeight(25);
    urlEdit->setPlaceholderText("Pesquise no Google ou digite um site...");

    favAddButton = new QPushButton(); styleBtn(favAddButton, "⭐", "Favoritar");
    favListButton = new QPushButton(); styleBtn(favListButton, "🔖", "Ver Favoritos");
    homeButton = new QPushButton(); styleBtn(homeButton, "🏠", "Ir para Home");
    historyButton = new QPushButton(); styleBtn(historyButton, "📜", "Histórico");
    privateModeButton = new QPushButton(); styleBtn(privateModeButton, "🕵", "Nova Janela Privada");
    downloadButton = new QPushButton(); styleBtn(downloadButton, "📥", "Downloads");

    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(urlEdit, 1);
    topLayout->addWidget(favAddButton);
    topLayout->addWidget(favListButton);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(historyButton);
    topLayout->addWidget(privateModeButton);
    topLayout->addWidget(downloadButton);

    // Gerenciador de Abas
    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    // Botão "+" (Posicionamento Dinâmico)
    addTabButton = new QPushButton("+", tabWidget);
    addTabButton->setFixedSize(20, 20);
    addTabButton->setCursor(Qt::PointingHandCursor);
    addTabButton->raise();

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);

    // CONEXÕES DE SINAIS
    connect(urlEdit, &QLineEdit::returnPressed, this, &BrowserWindow::handleUrlEntered);
    connect(addTabButton, &QPushButton::clicked, [this](){ createNewTab(); });
    
    connect(backButton, &QPushButton::clicked, [this](){ 
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back(); 
    });
    
    connect(refreshButton, &QPushButton::clicked, [this](){ 
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload(); 
    });

    connect(tabWidget->tabBar(), &QTabBar::currentChanged, [this](int){ syncTabButtonPos(); });
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, [this](int,int){ syncTabButtonPos(); });

    connect(tabWidget, &QTabWidget::tabCloseRequested, [this](int i){
        QWidget *w = tabWidget->widget(i);
        tabWidget->removeTab(i);
        w->deleteLater();
        if(tabWidget->count() == 0) createNewTab();
        syncTabButtonPos();
    });

    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(favAddButton, &QPushButton::clicked, this, &BrowserWindow::addFavorite);
    connect(favListButton, &QPushButton::clicked, this, &BrowserWindow::showFavoritesMenu);
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);
    connect(downloadButton, &QPushButton::clicked, this, &BrowserWindow::showDownloads);
    
    connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    loadSettings();
    createNewTab();
}

void BrowserWindow::handleUrlEntered() {
    QString input = urlEdit->text().trimmed();
    if(input.isEmpty()) return;

    QUrl url;
    // Lógica de Busca Minimalista: Se não houver ponto ou houver espaços, vai para o Google
    if (input.contains(" ") || (!input.contains(".") && !input.contains("://"))) {
        url = QUrl("https://www.google.com/search?q=" + input);
    } else {
        if (!input.contains("://")) input = "https://" + input;
        url = QUrl::fromUserInput(input);
    }

    if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        v->load(url);
    } else {
        // Se estiver na Home, troca a aba por uma Webview
        int idx = tabWidget->currentIndex();
        tabWidget->removeTab(idx);
        createNewTab(url);
    }
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

void BrowserWindow::resizeEvent(QResizeEvent *e) {
    QMainWindow::resizeEvent(e);
    syncTabButtonPos();
}

void BrowserWindow::createNewTab(const QUrl &url) {
    if (url.isEmpty()) {
        HomeView *home = new HomeView();
        int idx = tabWidget->addTab(home, QIcon("res/icons/piperos.png"), "Início");
        tabWidget->setCurrentIndex(idx);
        urlEdit->clear();
        connect(home, &HomeView::changeThemeRequested, this, &BrowserWindow::applyTheme);
        connect(home, &HomeView::changeBgRequested, this, &BrowserWindow::changeBackgroundImage);
    } else {
        QWebEngineView *view = new QWebEngineView();
        int idx = tabWidget->addTab(view, QIcon("res/icons/piperos.png"), "Carregando...");
        view->load(url);
        tabWidget->setCurrentIndex(idx);

        connect(view, &QWebEngineView::titleChanged, [this, view](const QString &t){
            int i = tabWidget->indexOf(view);
            if(i != -1) tabWidget->setTabText(i, t.isEmpty() ? "Nova Aba" : t);
        });
        
        connect(view, &QWebEngineView::urlChanged, [this, view](const QUrl &u){
            if(tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
            if(!m_isPrivate) {
                historyList.append(u.toString());
                if(historyList.size() > 1000) historyList.removeFirst();
                saveSettings();
            }
        });

        connect(view, &QWebEngineView::loadProgress, [this, view](int p){
            if(tabWidget->currentWidget() == view) {
                if(p < 100) refreshButton->setText("✖");
                else refreshButton->setText("↻");
            }
        });
    }
    syncTabButtonPos();
}

void BrowserWindow::goHome() {
    int idx = tabWidget->currentIndex();
    if(idx != -1) {
        tabWidget->removeTab(idx);
        createNewTab(QUrl()); 
    }
}

void BrowserWindow::updateIconsStyle(const QString &color, const QString &addButtonColor) {
    QString style = QString("QPushButton { color: %1; border: none; font-size: 19px; border-radius: 8px; background: transparent; } "
                            "QPushButton:hover { background: rgba(128,128,128,0.2); }").arg(color);
    
    backButton->setStyleSheet(style); refreshButton->setStyleSheet(style);
    homeButton->setStyleSheet(style); historyButton->setStyleSheet(style);
    favAddButton->setStyleSheet(style); favListButton->setStyleSheet(style);
    privateModeButton->setStyleSheet(style); downloadButton->setStyleSheet(style);

    // Botão + Especial (Contraste Máximo)
    QString addStyle = QString("QPushButton { color: white; border: none; font-size: 18px; font-weight: bold; border-radius: 6px; background: %1; } "
                               "QPushButton:hover { background: #111; }").arg(addButtonColor);
    addTabButton->setStyleSheet(addStyle);
}

void BrowserWindow::applyTheme(const QString &theme) {
    currentTheme = theme;
    saveSettings();

    if (theme == "dark") {
        this->setStyleSheet("QMainWindow { background: #121212; }");
        topBar->setStyleSheet("background: #1f1f1f; border-bottom: 1px solid #333;");
        urlEdit->setStyleSheet("background: #2c2c2c; color: white; border: 1px solid #444; border-radius: 8px; padding: 0 12px;");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #1f1f1f; width: 150px; color: #888; padding: 5px 15px; border: none; } "
                                 "QTabBar::tab:selected { background: #121212; color: white; border-bottom: 2px solid #4169E1; }");
        updateIconsStyle("white", "#444");
    } else if (theme == "light") {
        this->setStyleSheet("QMainWindow { background: white; }");
        topBar->setStyleSheet("background: #f8f9fa; border-bottom: 1px solid #ddd;");
        urlEdit->setStyleSheet("background: white; color: black; border: 1px solid #ccc; border-radius: 8px; padding: 0 12px;");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #e9ecef; width: 150px; color: #555; padding: 5px 15px; border: none; } "
                                 "QTabBar::tab:selected { background: white; color: black; border-bottom: 2px solid #4169E1; }");
        updateIconsStyle("black", "#888"); 
    } else { // DEFAULT (Piper Azul)
        this->setStyleSheet("QMainWindow { background: #f0f2f5; }");
        topBar->setStyleSheet("background: #4169E1; border: none;");
        urlEdit->setStyleSheet("background: white; color: black; border: none; border-radius: 8px; padding: 0 12px;");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #3456b8; width: 150px; color: white; padding: 5px 15px; border: none; } "
                                 "QTabBar::tab:selected { background: #f0f2f5; color: #4169E1; font-weight: bold; }");
        updateIconsStyle("white", "#27408B"); 
    }
    for(int i=0; i<tabWidget->count(); ++i) if(auto h = qobject_cast<HomeView*>(tabWidget->widget(i))) h->update();
}

void BrowserWindow::showHistory() {
    QDialog d(this); d.setWindowTitle("Histórico do Piper"); d.resize(500, 600);
    QVBoxLayout *l = new QVBoxLayout(&d);
    QListWidget *lw = new QListWidget();
    for(int i=historyList.size()-1; i>=0; --i) lw->addItem(historyList.at(i));
    l->addWidget(new QLabel("Dê um duplo clique para abrir o link:")); l->addWidget(lw);
    d.setStyleSheet("background: #1a1a1a; color: white;");
    connect(lw, &QListWidget::itemDoubleClicked, [this, &d](QListWidgetItem *it){
        createNewTab(QUrl(it->text())); d.accept();
    });
    d.exec();
}

void BrowserWindow::addFavorite() {
    if(auto v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        QString f = v->title() + "|" + v->url().toString();
        if(!favoritesList.contains(f)) { 
            favoritesList.append(f); saveSettings(); 
            favAddButton->setText("🌟"); QTimer::singleShot(1000, [this](){ favAddButton->setText("⭐"); });
        }
    }
}

void BrowserWindow::showFavoritesMenu() {
    QMenu m(this);
    m.setStyleSheet("QMenu { background: #fff; color: #333; } QMenu::item:selected { background: #4169E1; color: white; }");
    if(favoritesList.isEmpty()) {
        m.addAction("Nenhum favorito salvo");
    } else {
        for(const QString &f : favoritesList) {
            QStringList parts = f.split("|");
            m.addAction(parts.first(), [this, parts](){ createNewTab(QUrl(parts.last())); });
        }
    }
    m.exec(favListButton->mapToGlobal(QPoint(0, 40)));
}

void BrowserWindow::changeBackgroundImage() {
    QString p = QFileDialog::getOpenFileName(this, "Escolher Papel de Parede", "", "Imagens (*.png *.jpg *.jpeg *.bmp)");
    if(!p.isEmpty()) { 
        currentBgPath = p; 
        saveSettings(); 
        applyTheme(currentTheme); 
    }
}

void BrowserWindow::handleDownload(QWebEngineDownloadRequest *d) {
    QString p = QFileDialog::getSaveFileName(this, "Salvar Arquivo", QDir::homePath() + "/" + d->downloadFileName());
    if(p.isEmpty()) { d->cancel(); return; }
    d->setDownloadDirectory(QFileInfo(p).absolutePath());
    d->setDownloadFileName(QFileInfo(p).fileName());
    d->accept();
    m_downloads.append(d);
}

void BrowserWindow::showDownloads() {
    QMenu m(this);
    m.setStyleSheet("QMenu { background: #222; color: white; }");
    if(m_downloads.isEmpty()) m.addAction("Nenhum download na sessão");
    else for(auto *d : m_downloads) {
        QString status = (d->state() == QWebEngineDownloadRequest::DownloadCompleted) ? " (OK)" : " (Baixando...)";
        m.addAction(d->downloadFileName() + status);
    }
    m.exec(downloadButton->mapToGlobal(QPoint(0, 40)));
}

void BrowserWindow::openPrivateWindow() {
    QWebEngineProfile *p = new QWebEngineProfile(this);
    BrowserWindow *pw = new BrowserWindow(p);
    pw->m_isPrivate = true;
    pw->setWindowTitle("Piper (Modo Privado)");
    pw->applyTheme("dark");
    pw->show();
}

void BrowserWindow::loadSettings() {
    QSettings s("PiperOrg", "PiperBrowser");
    currentTheme = s.value("theme", "default").toString();
    currentBgPath = s.value("background", "").toString();
    favoritesList = s.value("favorites").toStringList();
    historyList = s.value("history").toStringList();
    applyTheme(currentTheme);
}

void BrowserWindow::saveSettings() {
    QSettings s("PiperOrg", "PiperBrowser");
    s.setValue("theme", currentTheme);
    s.setValue("background", currentBgPath);
    s.setValue("favorites", favoritesList);
    s.setValue("history", historyList);
}

BrowserWindow::~BrowserWindow() {}
void BrowserWindow::setThemeLight() { applyTheme("light"); }
void BrowserWindow::setThemeDark() { applyTheme("dark"); }
void BrowserWindow::setThemeDefault() { applyTheme("default"); }