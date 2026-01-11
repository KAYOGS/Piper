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
#include <QWebChannel>
#include <QStringList>
#include <QListWidget>
#include <QDialog>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include <QMenu>

class HomeView : public QWidget {
    Q_OBJECT
public:
    explicit HomeView(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event) override;
};

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();

public slots:
    void changeBackgroundImage();
    void applyDarkTheme();
    void addFavorite();
    void showFavoritesMenu();

private slots:
    void goHome();
    void openPrivateWindow();
    void showHistory();
    void showDownloads(); 
    void handleDownload(QWebEngineDownloadRequest *download);
    void createNewTab(const QUrl &url = QUrl());
    void loadSettings();
    void saveSettings();
    void syncTabButtonPos(); 
    void handleUrlEntered();
    void showAbout();
    void showMainMenu();
    
    void onTabChanged(int index);
    void checkTabActivity();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    QWidget *topBar;
    QTabWidget *tabWidget;
    QLineEdit *urlEdit;

    QPushButton *backButton, *refreshButton, *homeButton, *favAddButton, *menuButton;
    QPushButton *addTabButton;

    QStringList historyList;
    QStringList favoritesList; 
    QList<QWebEngineDownloadRequest*> m_downloads; 
    bool m_isPrivate;
    QString currentBgPath;
    QWebChannel *m_channel;

    QTimer *m_activityTimer;
    QMap<QWidget*, QDateTime> m_tabLastActivity;

    const int CAMADA_3_FROZEN = 600; 

    void updateIconsStyle();
    // Função auxiliar para configurar a WebView com conexões de ícone e título
    void setupWebView(QWebEngineView* view);
};

#endif
