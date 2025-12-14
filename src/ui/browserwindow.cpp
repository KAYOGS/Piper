#include "browserwindow.h"
#include <QWidget>
#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QListWidgetItem>
// Não precisamos mais de QWebSettings/QWebEngineSettings aqui, 
// pois removemos o código de otimização que estava causando o erro.

// Definição do Construtor
BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Piper");
    
    // --- 1. Configuração da Janela Principal e Layout ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    
    // Layout 100% minimalista: remove todas as margens e espaçamentos padrão
    mainLayout->setContentsMargins(0, 0, 0, 0); 
    mainLayout->setSpacing(0); 
    
    // --- 2. Barra Lateral de Abas (50px Fixos) ---
    sidebarTabs = new QListWidget();
    
    // Implementação dos requisitos de largura fixa:
    sidebarTabs->setMaximumWidth(50);
    sidebarTabs->setMinimumWidth(50); 
    
    // Estilização para Ícones e Performance:
    sidebarTabs->setViewMode(QListView::IconMode);
    sidebarTabs->setResizeMode(QListView::Fixed);
    sidebarTabs->setMovement(QListView::Static);
    sidebarTabs->setUniformItemSizes(true); 
    
    // Exemplo de uma aba:
    new QListWidgetItem("Home", sidebarTabs); 
    
    // --- 3. Web View (Área de Conteúdo) ---
    // NOVO: Instanciando a classe correta
    webView = new QWebEngineView();
    
    // *** CÓDIGO PROBLEMÁTICO ANTIGO FOI REMOVIDO AQUI ***

    // --- 4. Montar o Layout Horizontal ---
    mainLayout->addWidget(sidebarTabs); // Esquerda (50px)
    mainLayout->addWidget(webView);     // Direita (o restante)
    
    // --- 5. Carregamento da Página Inicial ---
    
    // Carrega o seu arquivo 'homepage.html' que está na pasta raiz 'Piper/'.
    // Usamos QDir::current().absoluteFilePath para garantir que funcione
    // independentemente do subdiretório 'build/bin' onde o executável é rodado.
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(QDir::current().absoluteFilePath("homepage.html"))));
}

// Definição do Destrutor
BrowserWindow::~BrowserWindow()
{
    // Limpeza de memória (gerenciada pelo Qt)
}