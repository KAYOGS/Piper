#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
// NOVO: Incluindo o header do motor WebEngine
#include <QtWebEngineWidgets/QWebEngineView> 
#include <QListWidget>            
#include <QHBoxLayout>            

class BrowserWindow : public QMainWindow
{
    Q_OBJECT 

public:
    BrowserWindow(QWidget *parent = nullptr);
    ~BrowserWindow();

private:
    // Componentes de Layout
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    
    // Componentes de Interface
    QListWidget *sidebarTabs; 
    // NOVO: Usando a classe de renderização do WebEngine
    QWebEngineView *webView;        
};

#endif // BROWSERWINDOW_H