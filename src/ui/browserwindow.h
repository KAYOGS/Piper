#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include <QProgressBar>
#include <QLabel>
#include <QStringList>

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Aceita um perfil (para modo privado) ou inicia com o padrão
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();

private slots:
    void goHome();
    void openPrivateWindow();
    void showHistory();
    void handleDownload(class QWebEngineDownloadRequest *download);
    void createNewTab(const QUrl &url); // Função para gerenciar múltiplas abas

private:
    // Layout e Containers
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    QWidget *topBar;
    QTabWidget *tabWidget;
    QLineEdit *urlEdit;

    // Botões
    QPushButton *backButton;
    QPushButton *refreshButton;
    QPushButton *addTabButton;
    QPushButton *homeButton;
    QPushButton *historyButton;
    QPushButton *privateModeButton;

    // Download UI
    QWidget *downloadWidget;
    QProgressBar *downloadBar;

    // Dados
    QStringList historyList;
    bool m_isPrivate;
};

#endif // BROWSERWINDOW_H