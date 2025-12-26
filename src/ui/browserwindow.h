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
#include <QSettings>
#include <QWebChannel> // Necessário para comunicação JS -> C++

class BrowserWindow : public QMainWindow
{
    Q_OBJECT
    // Propriedades expostas para o JavaScript
    Q_PROPERTY(QString currentTheme READ getTheme WRITE applyTheme NOTIFY themeChanged)

public:
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();

    QString getTheme() const { return currentTheme; }

public slots:
    // Funções chamadas pelo botão de engrenagem no HTML
    void changeBackgroundImage();
    void setThemeLight();
    void setThemeDark();
    void setThemeDefault();
    void applyTheme(const QString &themeName);

signals:
    void themeChanged();

private slots:
    void goHome();
    void openPrivateWindow();
    void showHistory();
    void showDownloads(); 
    void handleDownload(QWebEngineDownloadRequest *download);
    void createNewTab(const QUrl &url = QUrl());
    void loadSettings();
    void saveSettings();

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
    QList<QWebEngineDownloadRequest*> m_downloads; 
    bool m_isPrivate;

    QString currentTheme;
    QString currentBgPath;
    QWebChannel *m_channel; // Canal de comunicação
};

#endif // BROWSERWINDOW_H