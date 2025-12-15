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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStringLiteral> // Necessário para o construtor QStringLiteral

// ----------------------------------------------------------------------------------
// Definição do Construtor
// ----------------------------------------------------------------------------------

BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent)
{
    // --- Configurações Iniciais ---
    
    // MUDANÇA: O título reflete o modo de navegação
    if (profile) {
        setWindowTitle("Piper - Modo Privado");
    } else {
        setWindowTitle("Piper - browser"); 
    }
    
    // O ÍCONE FOI REMOVIDO TEMPORARIAMENTE para evitar erros de compilação com o QRC
    // setWindowIcon(QIcon(":/icons/piperos.png")); 
    
    // --- 1. Configuração da Janela Principal e Layout ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    
    // Layout 100% minimalista: remove todas as margens e espaçamentos padrão
    mainLayout->setContentsMargins(0, 0, 0, 0); 
    mainLayout->setSpacing(0); 
    
    // --- 2. Barra Lateral de Abas (50px Fixos) ---
    
    // 2a. Container Principal e Layout Vertical
    QWidget *sidebarContainer = new QWidget();
    sidebarContainer->setMaximumWidth(50);
    sidebarContainer->setMinimumWidth(50); 
    
    // Configuração de Layout Vertical para empilhar abas e botões
    sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(0, 5, 0, 0); // Padding superior
    sidebarLayout->setSpacing(5); // Espaçamento entre os widgets

    // 2b. Área das Abas (QListWidget Expansível)
    sidebarTabs = new QListWidget();
    sidebarTabs->setViewMode(QListView::IconMode);
    sidebarTabs->setResizeMode(QListView::Fixed);
    sidebarTabs->setMovement(QListView::Static);
    sidebarTabs->setUniformItemSizes(true); 
    sidebarTabs->setFixedWidth(50); // Garantir largura dos ícones
    sidebarTabs->setStyleSheet("QListWidget { border: none; }"); 
    sidebarLayout->addWidget(sidebarTabs); // Adicionado ao topo

    // O ÍCONE DE EXEMPLO FOI REMOVIDO TEMPORARIAMENTE
    // new QListWidgetItem(QIcon(":/icons/home.png"), "", sidebarTabs); 
    
    // 2c. Área de Botões Fixos (Bottom)
    fixedButtonsContainer = new QWidget();
    fixedButtonsLayout = new QVBoxLayout(fixedButtonsContainer);
    fixedButtonsLayout->setContentsMargins(0, 5, 0, 5); // Padding interno
    fixedButtonsLayout->setSpacing(2); 
    
    // Criação dos Botões (AGORA COM TEXTO E QStringLiteral PARA CORRIGIR O ERRO)
    privateModeButton = new QPushButton(QStringLiteral("Priv"));
    homeButton = new QPushButton(QStringLiteral("Home"));
    historyButton = new QPushButton(QStringLiteral("Hist"));
    passwordsButton = new QPushButton(QStringLiteral("Pass"));

    // Conexões
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome); // NOVO: Conexão Home
    // *Outros botões (Hist, Pass) serão conectados depois*

    // Adiciona os botões ao layout fixo (na ordem: Privado, Home, Histórico, Senha)
    fixedButtonsLayout->addWidget(privateModeButton); 
    fixedButtonsLayout->addWidget(homeButton);
    fixedButtonsLayout->addWidget(historyButton);
    fixedButtonsLayout->addWidget(passwordsButton);
    
    // Adiciona o container de botões fixos ao final do layout vertical da sidebar
    sidebarLayout->addWidget(fixedButtonsContainer);


    // --- 3. Web View (Área de Conteúdo) e Lógica de Perfil ---
    
    // Lógica do Perfil
    if (profile == nullptr) {
        privateProfile = new QWebEngineProfile("private_mode", this);
        privateProfile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
        privateProfile->setPersistentStoragePath(""); 
        
        // Cria a QWebEngineView usando o perfil padrão (nullptr)
        webView = new QWebEngineView();
        
    } else {
        privateProfile = nullptr; 

        QWebEnginePage *privatePage = new QWebEnginePage(profile, this);
        webView = new QWebEngineView();
        webView->setPage(privatePage);
    }
    
    // --- 4. Montar o Layout Horizontal ---
    mainLayout->addWidget(sidebarContainer); // Esquerda (50px)
    mainLayout->addWidget(webView);          // Direita (o restante)
    
    // --- 5. Carregamento da Página Inicial ---
    
    // CHAMA A FUNÇÃO DE NAVEGAÇÃO HOME ASSIM QUE A JANELA ABRE
    goHome();
}

// ----------------------------------------------------------------------------------
// Definição do Destrutor
// ----------------------------------------------------------------------------------

BrowserWindow::~BrowserWindow()
{
    // Limpeza de memória (gerenciada pelo Qt)
}

// ----------------------------------------------------------------------------------
// Slot: openPrivateWindow
// ----------------------------------------------------------------------------------

void BrowserWindow::openPrivateWindow()
{
    // Verifica se o perfil privado foi criado (só acontece na janela principal)
    if (privateProfile) {
        // Cria uma nova instância da BrowserWindow, passando o perfil privado
        BrowserWindow *privateWindow = new BrowserWindow(privateProfile);
        privateWindow->show();
    }
    // Se privateProfile for nulo, a janela atual já é privada e não deve criar outra
}

// ----------------------------------------------------------------------------------
// Slot: goHome (NOVO)
// ----------------------------------------------------------------------------------

void BrowserWindow::goHome()
{
    // Carrega o URL do Google como Home Page Provisória (até implementarmos o HTML local)
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(QDir::current().absoluteFilePath("homepage.html"))));
}