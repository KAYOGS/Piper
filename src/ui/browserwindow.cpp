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

    sidebarLayout->addStretch(1);

    // --- Botão Home (Substituído Texto por Ícone) ---
    homeButton = new QPushButton(); 
    homeButton->setFixedSize(40, 40);
    
    // Carrega o ícone da Piper
    QIcon homeIcon(QDir::current().absoluteFilePath("res/icons/home.png"));
    homeButton->setIcon(homeIcon);
    homeButton->setIconSize(QSize(32, 32)); // Tamanho do ícone dentro do botão

    // Estilo VS Code: Fundo transparente que aparece apenas no hover
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

    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    sidebarLayout->addWidget(homeButton, 0, Qt::AlignCenter);

    // --- 3. Web View (Área de Conteúdo) ---

    QString firefoxUserAgent = QStringLiteral("Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0");
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(firefoxUserAgent);

    webView = new QWebEngineView();
    
    // Permissões de navegação para evitar erros de rede
    webView->page()->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    // Título dinâmico da janela
    connect(webView, &QWebEngineView::titleChanged, this, [this](const QString &title)
            {
        if(title.isEmpty() || title == "PiperHub") {
            setWindowTitle("Piper - browser");
        } else {
            setWindowTitle(title + " - Piper");
        } });

    // --- 4. Montar o Layout Horizontal ---
    mainLayout->addWidget(sidebarContainer);
    mainLayout->addWidget(webView);

    // --- 5. Inicialização ---
    goHome();
    showMaximized();
}

// ----------------------------------------------------------------------------------
// Definição do Destrutor
// ----------------------------------------------------------------------------------

BrowserWindow::~BrowserWindow()
{
}

// ----------------------------------------------------------------------------------
// Slot: goHome
// ----------------------------------------------------------------------------------

void BrowserWindow::goHome()
{
    QString path = QDir::current().absoluteFilePath("homepage.html");
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(path)));
}