#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QWidget>
#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStringLiteral>
#include <QDebug> 

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- Configurações de Identidade do Piper ---
    setWindowTitle("Piper - browser");
    setWindowIcon(QIcon(QDir::current().absoluteFilePath("res/icons/piperos.png")));

    // --- 1. Layout Principal ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 2. Barra Lateral de Controle ---
    QWidget *sidebarContainer = new QWidget();
    sidebarContainer->setMaximumWidth(50);
    sidebarContainer->setMinimumWidth(50);

    sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(0, 5, 0, 5);
    sidebarLayout->setSpacing(5);

    sidebarTabs = new QListWidget();
    sidebarTabs->setViewMode(QListView::IconMode);
    sidebarTabs->setResizeMode(QListView::Fixed);
    sidebarTabs->setMovement(QListView::Static);
    sidebarTabs->setUniformItemSizes(true);
    sidebarTabs->setFixedWidth(50);
    sidebarTabs->setStyleSheet("QListWidget { border: none; background-color: #252526; }");
    sidebarLayout->addWidget(sidebarTabs);

    sidebarLayout->addStretch(1);

    // --- 3. Botões da Interface ---
    backButton = new QPushButton();
    backButton->setFixedSize(40, 40);
    backButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/voltar.png")));
    backButton->setIconSize(QSize(32, 32));
    backButton->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 5px; } QPushButton:hover { background-color: #3c3c3c; }");

    refreshButton = new QPushButton();
    refreshButton->setFixedSize(40, 40);
    refreshButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/recarregar.png")));
    refreshButton->setIconSize(QSize(32, 32));
    refreshButton->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 5px; } QPushButton:hover { background-color: #3c3c3c; }");

    historyButton = new QPushButton();
    historyButton->setFixedSize(40, 40);
    historyButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/history.png")));
    historyButton->setIconSize(QSize(32, 32));
    historyButton->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 5px; } QPushButton:hover { background-color: #3c3c3c; }");

    homeButton = new QPushButton();
    homeButton->setFixedSize(40, 40);
    homeButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/home.png")));
    homeButton->setIconSize(QSize(32, 32));
    homeButton->setStyleSheet("QPushButton { background-color: transparent; border: none; border-radius: 5px; } QPushButton:hover { background-color: #3c3c3c; }");

    sidebarLayout->addWidget(backButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(refreshButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(historyButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(homeButton, 0, Qt::AlignCenter);

    // --- 4. Motor do Escudo (Configuração de Perfil) ---
    QWebEngineProfile *profile = QWebEngineProfile::defaultProfile();
    
    QString chromeUserAgent = QStringLiteral("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    profile->setHttpUserAgent(chromeUserAgent);
    profile->setPersistentStoragePath(QDir::currentPath() + "/storage");
    profile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    // Vinculação do Interceptor de URLs
    UrlInterceptor *interceptor = new UrlInterceptor(this);
    profile->setUrlRequestInterceptor(interceptor);

    // --- 5. Criação da Página Protegida ---
    QWebEnginePage *piperPage = new QWebEnginePage(profile, this);
    webView = new QWebEngineView(this);
    webView->setPage(piperPage);
    
    // Configurações de Performance
    webView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    // --- 6. Conexões e Sinais ---
    connect(backButton, &QPushButton::clicked, webView, &QWebEngineView::back);
    connect(refreshButton, &QPushButton::clicked, webView, &QWebEngineView::reload);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);

    connect(webView, &QWebEngineView::urlChanged, this, [this](const QUrl &url) {
        QString urlStr = url.toString();
        QString homePath = QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")).toString();
        
        if (urlStr != homePath && !urlStr.isEmpty() && !urlStr.startsWith("data:text/html") && 
           (historyList.isEmpty() || historyList.last() != urlStr)) {
            historyList.append(urlStr);
            if (historyList.size() > 100) historyList.removeFirst();
            qDebug() << "Histórico Piper:" << urlStr;
        }
    });

    connect(webView, &QWebEngineView::titleChanged, this, [this](const QString &title) {
        if(title.isEmpty() || title == "PiperHub") {
            setWindowTitle("Piper - browser");
        } else {
            setWindowTitle(title + " - Piper");
        } 
    });

    mainLayout->addWidget(sidebarContainer);
    mainLayout->addWidget(webView);

    goHome();
    showMaximized();
}

void BrowserWindow::showHistory()
{
    QString html = "<html><head><style>"
                   "body { background-color: #1e1e1e; color: white; font-family: 'Segoe UI', sans-serif; padding: 50px; }"
                   "h1 { color: #469ECE; border-bottom: 2px solid #3c3c3c; padding-bottom: 15px; }"
                   ".item { background: #2d2d2d; padding: 15px; border-radius: 10px; margin-bottom: 12px; transition: 0.3s; border-left: 5px solid transparent; }"
                   ".item:hover { background: #3c3c3c; border-left: 5px solid #469ECE; transform: translateX(10px); cursor: pointer; }"
                   ".title { font-weight: bold; font-size: 16px; margin-bottom: 5px; color: #fff; }"
                   ".url { color: #888; font-size: 13px; text-decoration: none; word-break: break-all; }"
                   ".empty { text-align: center; color: #555; font-size: 20px; margin-top: 100px; }"
                   "</style></head><body>"
                   "<h1>Histórico de Navegação</h1>";

    if (historyList.isEmpty()) {
        html += "<div class='empty'>Nenhum site visitado ainda.</div>";
    } else {
        for (int i = historyList.size() - 1; i >= 0; --i) {
            QString url = historyList.at(i);
            html += QString("<div class='item' onclick='window.location=\"%1\"'>"
                            "<div class='title'>Site Visitado</div>"
                            "<div class='url'>%1</div>"
                            "</div>").arg(url);
        }
    }

    html += "</body></html>";
    webView->setHtml(html);
}

void BrowserWindow::goHome()
{
    QString path = QDir::current().absoluteFilePath("homepage.html");
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(path)));
}

BrowserWindow::~BrowserWindow() {}