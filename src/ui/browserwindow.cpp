#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QDir>
#include <QIcon>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QWebEngineDownloadRequest>
#include <QFileInfo>
#include <QDesktopServices>
#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QWebChannel>
#include <QMenu>
#include <QAction>

// --- Construtor Principal do Piper ---
BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent), m_isPrivate(false)
{
    if (!profile) profile = QWebEngineProfile::defaultProfile();
    
    // 1. Instalação da Sniper (UrlInterceptor)
    profile->setUrlRequestInterceptor(new UrlInterceptor(this));

    // 2. Configuração do Canal de Comunicação (C++ <-> JS)
    m_channel = new QWebChannel(this);
    m_channel->registerObject(QStringLiteral("piper"), this);

    // 3. Configuração do Motor de Performance
    QWebEngineSettings *s = profile->settings();
    s->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    s->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    s->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);

    // --- Layout e UI ---
    setWindowTitle("Piper Browser");
    showMaximized();

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // BARRA SUPERIOR
    topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(50);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);
    topLayout->setSpacing(8);

    backButton = new QPushButton("←");
    refreshButton = new QPushButton("↻");
    
    urlEdit = new QLineEdit();
    urlEdit->setPlaceholderText("Pesquisar ou digitar URL...");

    addTabButton = new QPushButton("+");
    homeButton = new QPushButton("🏠");
    historyButton = new QPushButton("📜");
    privateModeButton = new QPushButton("🕵");
    downloadButton = new QPushButton("📥");

    // Adicionando Widgets na Barra
    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(urlEdit, 1); 
    topLayout->addWidget(addTabButton);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(historyButton);
    topLayout->addWidget(privateModeButton);
    topLayout->addWidget(downloadButton);

    tabWidget = new QTabWidget();
    tabWidget->setObjectName("mainTabs");
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);

    // Conexões de Navegação
    connect(urlEdit, &QLineEdit::returnPressed, [=]() {
        QString text = urlEdit->text().trimmed();
        if(!text.contains(".")) text = "https://www.google.com/search?q=" + text;
        else if(!text.startsWith("http")) text = "https://" + text;
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->load(QUrl(text));
    });

    connect(backButton, &QPushButton::clicked, [=]() { if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back(); });
    connect(refreshButton, &QPushButton::clicked, [=]() { if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload(); });
    connect(addTabButton, &QPushButton::clicked, [=]() { createNewTab(); });
    
    connect(tabWidget, &QTabWidget::tabCloseRequested, [=](int index) { 
        QWidget *w = tabWidget->widget(index);
        tabWidget->removeTab(index);
        w->deleteLater();
        if(tabWidget->count() == 0) createNewTab(); 
    });

    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(downloadButton, &QPushButton::clicked, this, &BrowserWindow::showDownloads);
    connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    // Carregar configurações salvas e abrir aba inicial
    loadSettings();
    createNewTab();
}

BrowserWindow::~BrowserWindow() {}

// --- Persistência Nativa (QSettings) ---
void BrowserWindow::loadSettings() {
    QSettings settings("PiperOrg", "PiperBrowser");
    currentTheme = settings.value("theme", "default").toString();
    currentBgPath = settings.value("background", "").toString();
    applyTheme(currentTheme);
}

void BrowserWindow::saveSettings() {
    QSettings settings("PiperOrg", "PiperBrowser");
    settings.setValue("theme", currentTheme);
    settings.setValue("background", currentBgPath);
}

// --- Slots de Customização ---
void BrowserWindow::setThemeLight() { applyTheme("light"); }
void BrowserWindow::setThemeDark() { applyTheme("dark"); }
void BrowserWindow::setThemeDefault() { applyTheme("default"); }

void BrowserWindow::changeBackgroundImage() {
    QString path = QFileDialog::getOpenFileName(this, "Selecionar Imagem de Fundo", 
                   QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                   "Imagens (*.png *.jpg *.jpeg *.bmp)");
    if (!path.isEmpty()) {
        currentBgPath = path;
        saveSettings();
        goHome(); 
    }
}

void BrowserWindow::applyTheme(const QString &themeName) {
    currentTheme = themeName;
    saveSettings();

    QString btnStyle = "QPushButton { border: none; font-size: 18px; padding: 5px; border-radius: 4px; background: transparent; } "
                       "QPushButton:hover { background-color: rgba(0, 0, 0, 0.1); }";

    if (themeName == "dark") {
        this->setStyleSheet("QMainWindow { background-color: #121212; }");
        topBar->setStyleSheet("background-color: #1f1f1f; border-bottom: 1px solid #333;");
        urlEdit->setStyleSheet("QLineEdit { background: #2c2c2c; color: #fff; border: 1px solid #444; border-radius: 4px; padding: 4px 10px; }");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #1f1f1f; color: #bbb; padding: 6px 15px; } "
                                 "QTabBar::tab:selected { background: #121212; color: #fff; }");
        QString darkBtn = btnStyle + "QPushButton { color: #fff; }";
        backButton->setStyleSheet(darkBtn); refreshButton->setStyleSheet(darkBtn); addTabButton->setStyleSheet(darkBtn);
        homeButton->setStyleSheet(darkBtn); historyButton->setStyleSheet(darkBtn); privateModeButton->setStyleSheet(darkBtn);
        downloadButton->setStyleSheet(darkBtn);
    } 
    else if (themeName == "light") {
        this->setStyleSheet("QMainWindow { background-color: #ffffff; }");
        topBar->setStyleSheet("background-color: #f5f5f5; border-bottom: 1px solid #ddd;");
        urlEdit->setStyleSheet("QLineEdit { background: #fff; color: #000; border: 1px solid #ccc; border-radius: 4px; padding: 4px 10px; }");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #e0e0e0; color: #333; padding: 6px 15px; } "
                                 "QTabBar::tab:selected { background: #fff; color: #000; }");
        QString lightBtn = btnStyle + "QPushButton { color: #000; }";
        backButton->setStyleSheet(lightBtn); refreshButton->setStyleSheet(lightBtn); addTabButton->setStyleSheet(lightBtn);
        homeButton->setStyleSheet(lightBtn); historyButton->setStyleSheet(lightBtn); privateModeButton->setStyleSheet(lightBtn);
        downloadButton->setStyleSheet(lightBtn);
    } 
    else { // DEFAULT (AZUL ROYAL)
        this->setStyleSheet("QMainWindow { background-color: #f0f0f0; }");
        topBar->setStyleSheet("background-color: #4169E1; border-bottom: 1px solid #27408B;");
        urlEdit->setStyleSheet("QLineEdit { background: #f0f0f0; color: #000; border: 1px solid #27408B; border-radius: 4px; padding: 4px 10px; }");
        tabWidget->setStyleSheet("QTabWidget::pane { border: none; } QTabBar::tab { background: #3a5ed0; color: #fff; padding: 6px 15px; } "
                                 "QTabBar::tab:selected { background: #f0f0f0; color: #4169E1; font-weight: bold; }");
        QString defBtn = btnStyle + "QPushButton { color: #fff; }";
        backButton->setStyleSheet(defBtn); refreshButton->setStyleSheet(defBtn); addTabButton->setStyleSheet(defBtn);
        homeButton->setStyleSheet(defBtn); historyButton->setStyleSheet(defBtn); privateModeButton->setStyleSheet(defBtn);
        downloadButton->setStyleSheet(defBtn);
    }
    emit themeChanged();
}

void BrowserWindow::createNewTab(const QUrl &url) {
    QWebEngineView *view = new QWebEngineView();
    view->page()->setWebChannel(m_channel); 

    if (url.isEmpty()) {
        QString homePath = QDir::current().absoluteFilePath("homepage.html");
        view->load(QUrl::fromLocalFile(homePath));
    } else {
        view->load(url);
    }

    connect(view, &QWebEngineView::loadFinished, [this, view](bool ok) {
        if (ok && !currentBgPath.isEmpty()) {
            QString js = QString("document.body.style.backgroundImage = \"url('file://%1')\"; "
                                 "document.body.style.backgroundSize = 'cover'; "
                                 "document.body.style.backgroundAttachment = 'fixed';").arg(currentBgPath);
            view->page()->runJavaScript(js);
        }
    });

    int index = tabWidget->addTab(view, QIcon("res/icons/piperos.png"), "Piper");
    tabWidget->setCurrentIndex(index);

    connect(view, &QWebEngineView::titleChanged, [=](const QString &title) {
        int idx = tabWidget->indexOf(view);
        if(idx != -1) tabWidget->setTabText(idx, title.left(15));
    });

    connect(view, &QWebEngineView::urlChanged, [=](const QUrl &u) {
        if(tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if(!m_isPrivate && !u.toString().contains("homepage.html")) historyList.append(u.toString());
    });
}

void BrowserWindow::handleDownload(QWebEngineDownloadRequest *download) {
    QString path = QFileDialog::getSaveFileName(this, "Salvar Arquivo", 
                   QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + download->downloadFileName());
    
    if (path.isEmpty()) {
        download->cancel();
        return;
    }

    QFileInfo fi(path);
    download->setDownloadDirectory(fi.absolutePath());
    download->setDownloadFileName(fi.fileName());
    
    download->accept();
    m_downloads.append(download);
}

void BrowserWindow::showDownloads() {
    QMenu *menu = new QMenu(this);
    if (m_downloads.isEmpty()) {
        menu->addAction("Nenhum download recente.");
    } else {
        for (int i = m_downloads.size() - 1; i >= 0; --i) {
            auto *d = m_downloads.at(i);
            QString status = (d->state() == QWebEngineDownloadRequest::DownloadInProgress) ? " (Baixando...)" : " (OK)";
            QAction *act = menu->addAction(d->downloadFileName() + status);
            connect(act, &QAction::triggered, [d]() { QDesktopServices::openUrl(QUrl::fromLocalFile(d->downloadDirectory())); });
        }
    }
    menu->exec(downloadButton->mapToGlobal(QPoint(0, downloadButton->height() + 5)));
}

void BrowserWindow::openPrivateWindow() {
    QWebEngineProfile *privateProfile = new QWebEngineProfile(this);
    BrowserWindow *privateWin = new BrowserWindow(privateProfile);
    privateWin->m_isPrivate = true;
    privateWin->setStyleSheet("QMainWindow { border: 3px solid #4B0082; }");
    privateWin->show();
}

void BrowserWindow::goHome() {
    if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        QString homePath = QDir::current().absoluteFilePath("homepage.html");
        v->load(QUrl::fromLocalFile(homePath));
    }
}

void BrowserWindow::showHistory() {
    QString html = "<html><body style='background:#2b4491; font-family:sans-serif; color:white; padding:20px;'>"
                   "<h1>Histórico Piper</h1>";
    for(int i = historyList.size()-1; i >= 0; --i) {
        html += QString("<div style='background:rgba(255,255,255,0.1); padding:10px; margin-bottom:5px; border-radius:4px;'>%1</div>").arg(historyList.at(i));
    }
    html += "</body></html>";
    
    QWebEngineView *historyView = new QWebEngineView();
    historyView->setHtml(html);
    tabWidget->addTab(historyView, "Histórico");
    tabWidget->setCurrentWidget(historyView);
}