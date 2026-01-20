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
#include <QStackedWidget>
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
#include <QScrollArea>
#include <QGridLayout>

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
    void handleUrlEntered();
    void showAbout();
    void showMainMenu();
    void toggleTabSwitcher();
    void checkTabActivity();
    void clearAllTabs();

protected:
    void resizeEvent(QResizeEvent *event) override;
    // Esta é a linha que estava faltando e causou o erro:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    QWidget *topBar;
    QStackedWidget *stackedWidget;
    QLineEdit *urlEdit;

    QPushButton *backButton, *refreshButton, *homeButton, *favAddButton, *menuButton;
    QPushButton *tabsButton, *addTabButton;

    QWidget *tabSwitcherWidget;
    QScrollArea *tabScrollArea;
    QWidget *tabGridContainer;
    QGridLayout *tabGridLayout;

    QStringList historyList;
    QStringList favoritesList; 
    QList<QWebEngineDownloadRequest*> m_downloads; 
    bool m_isPrivate;
    QString currentBgPath;
    QWebChannel *m_channel;
    QTimer *m_activityTimer;

    struct TabInfo {
        QWidget* widget;
        QString title;
        QPixmap thumbnail;
    };
    QList<TabInfo> m_tabs;

    void updateIconsStyle();
    void setupWebView(QWebEngineView* view);
    void updateCurrentThumbnail();
    void refreshTabGrid();
    void updateUrlBar(int index);
};

#endif // BROWSERWINDOW_H