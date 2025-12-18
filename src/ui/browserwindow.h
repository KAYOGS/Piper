#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile> // Manter para usar o perfil padrão
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Construtor sem o argumento opcional QWebEngineProfile
    explicit BrowserWindow(QWidget *parent = nullptr); 
    ~BrowserWindow();

private:
    // Componentes de Layout
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *sidebarLayout;
    QListWidget *sidebarTabs;
    QPushButton *refreshButton;
    QStringList historyList;
    QPushButton *backButton;

    // Componentes de Navegação
    QWebEngineView *webView;
    // REMOVIDO: QWebEngineProfile *privateProfile; 

    // Botões da Barra Lateral (SOMENTE Home)
    QPushButton *homeButton;

private slots: 
    void goHome(); 

};

#endif // BROWSERWINDOW_H