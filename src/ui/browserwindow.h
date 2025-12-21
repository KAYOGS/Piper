#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEngineDownloadRequest>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>
#include <QLabel>
#include <QStringList>
#include <QList>
#include <QMenu>

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();

private slots:
    void goHome();
    void openPrivateWindow();
    void showHistory();
    void showDownloads(); 
    void handleDownload(QWebEngineDownloadRequest *download);
    void createNewTab(const QUrl &url = QUrl());

private:
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    QWidget *topBar;
    QTabWidget *tabWidget;
    QLineEdit *urlEdit;

    QPushButton *backButton;
    QPushButton *refreshButton;
    QPushButton *addTabButton;
    QPushButton *homeButton;
    QPushButton *historyButton;
    QPushButton *privateModeButton;
    QPushButton *downloadButton;

    QStringList historyList;
    // Lista para gerenciar os arquivos na memória e evitar erros de ponteiro
    QList<QWebEngineDownloadRequest*> m_downloads; 
    bool m_isPrivate;
};

#endif // BROWSERWINDOW_H