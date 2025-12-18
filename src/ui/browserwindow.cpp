#include "browserwindow.h"
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

// ----------------------------------------------------------------------------------
// Definição do Construtor
// ----------------------------------------------------------------------------------

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- Configurações Iniciais ---

    setWindowTitle("Piper - browser");
    // Define o ícone da janela (que aparece na barra de tarefas)
    setWindowIcon(QIcon(QDir::current().absoluteFilePath("res/icons/piperos.png")));

    // --- 1. Configuração da Janela Principal e Layout ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 2. Barra Lateral de Abas (50px Fixos) ---

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

    // Empurra os botões para a parte inferior da barra lateral
    sidebarLayout->addStretch(1);

    // --- NOVO: Botão Voltar ---
    backButton = new QPushButton();
    backButton->setFixedSize(40, 40);
    QIcon backIcon(QDir::current().absoluteFilePath("res/icons/voltar.png"));
    backButton->setIcon(backIcon);
    backButton->setIconSize(QSize(32, 32));
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c3c3c;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #007acc;"
        "}"
    );

    // --- Botão de Refresh com Ícone ---
    refreshButton = new QPushButton();
    refreshButton->setFixedSize(40, 40);
    QIcon refreshIcon(QDir::current().absoluteFilePath("res/icons/recarregar.png"));
    refreshButton->setIcon(refreshIcon);
    refreshButton->setIconSize(QSize(32, 32));
    refreshButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c3c3c;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #007acc;"
        "}"
    );

    // --- Botão Home ---
    homeButton = new QPushButton();
    homeButton->setFixedSize(40, 40);
    QIcon homeIcon(QDir::current().absoluteFilePath("res/icons/home.png"));
    homeButton->setIcon(homeIcon);
    homeButton->setIconSize(QSize(32, 32));
    homeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c3c3c;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #007acc;"
        "}"
    );

    // Adiciona os botões à barra lateral na ordem sugerida
    sidebarLayout->addWidget(backButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(refreshButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(homeButton, 0, Qt::AlignCenter);

    // --- 3. Configuração do Perfil e Web View ---

    QString chromeUserAgent = QStringLiteral("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36");
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(chromeUserAgent);

    QWebEngineProfile::defaultProfile()->setPersistentStoragePath(QDir::currentPath() + "/storage");
    QWebEngineProfile::defaultProfile()->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);

    webView = new QWebEngineView();
    
    webView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    // --- Conexões de Sinais e Slots ---

    // Ações de Navegação
    connect(backButton, &QPushButton::clicked, webView, &QWebEngineView::back);
    connect(refreshButton, &QPushButton::clicked, webView, &QWebEngineView::reload);
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);

    // Lógica de Histórico Filtrado (Ignora a Homepage)
    connect(webView, &QWebEngineView::urlChanged, this, [this](const QUrl &url) {
        QString urlStr = url.toString();
        QString homePath = QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")).toString();

        // Só salva no histórico se não for a home e não for repetido
        if (urlStr != homePath && !urlStr.isEmpty() && (historyList.isEmpty() || historyList.last() != urlStr)) {
            historyList.append(urlStr);
            if (historyList.size() > 100) historyList.removeFirst();
            qDebug() << "Histórico Piper:" << urlStr;
        }
    });

    // Atualiza o título da janela conforme a navegação
    connect(webView, &QWebEngineView::titleChanged, this, [this](const QString &title)
    {
        if(title.isEmpty() || title == "PiperHub") {
            setWindowTitle("Piper - browser");
        } else {
            setWindowTitle(title + " - Piper");
        } 
    });

    // --- 4. Montar o Layout Horizontal ---
    mainLayout->addWidget(sidebarContainer);
    mainLayout->addWidget(webView);

    // --- 5. Inicialização ---
    goHome();
    showMaximized();
}

BrowserWindow::~BrowserWindow() {}

void BrowserWindow::goHome()
{
    QString path = QDir::current().absoluteFilePath("homepage.html");
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(path)));
}