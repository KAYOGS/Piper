#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QWidget>

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();

private:
    // Componentes de Layout
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QVBoxLayout *sidebarLayout;
    QListWidget *sidebarTabs;
    QWidget *fixedButtonsContainer;
    QVBoxLayout *fixedButtonsLayout;

    // Componentes de Navegação
    QWebEngineView *webView;
    QWebEngineProfile *privateProfile; // Perfil usado para janelas privadas

    // Botões da Barra Lateral
    QPushButton *privateModeButton;
    QPushButton *homeButton;
    QPushButton *historyButton;
    QPushButton *passwordsButton;

private slots: // SEÇÃO NOVO/CORRIGIDA
    void openPrivateWindow();
    void goHome(); // NOVO SLOT: Para o botão Home

};

#endif // BROWSERWINDOW_H