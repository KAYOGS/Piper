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

// Dashboard Minimalista
class HomeView : public QWidget {
    Q_OBJECT
public:
    explicit HomeView(QWidget *parent = nullptr);
signals:
    void changeThemeRequested(const QString &theme);
    void changeBgRequested();
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    QPushButton *settingsBtn;
};

class BrowserWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ getTheme WRITE applyTheme NOTIFY themeChanged)

public:
    explicit BrowserWindow(QWebEngineProfile *profile = nullptr, QWidget *parent = nullptr);
    ~BrowserWindow();
    QString getTheme() const { return currentTheme; }

public slots:
    void changeBackgroundImage();
    void setThemeLight();
    void setThemeDark();
    void setThemeDefault();
    void applyTheme(const QString &themeName);
    void addFavorite();
    void showFavoritesMenu();

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
    void syncTabButtonPos(); 
    void handleUrlEntered();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QWidget *centralWidget;
    QVBoxLayout *rootLayout;
    QWidget *topBar;
    QTabWidget *tabWidget;
    QLineEdit *urlEdit;

    QPushButton *backButton, *refreshButton, *addTabButton;
    QPushButton *homeButton, *historyButton, *privateModeButton, *downloadButton;
    QPushButton *favAddButton, *favListButton;

    QStringList historyList;
    QStringList favoritesList; 
    QList<QWebEngineDownloadRequest*> m_downloads; 
    bool m_isPrivate;
    QString currentTheme, currentBgPath;
    QWebChannel *m_channel;

    void updateIconsStyle(const QString &color, const QString &addButtonColor = "");
};

#endif