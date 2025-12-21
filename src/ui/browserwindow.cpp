#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QDir>
#include <QIcon>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QWebEngineDownloadRequest>
#include <QFileInfo>
#include <QDesktopServices>
#include <QWebEngineSettings>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QCoreApplication>
#include <QDebug>
#include <fstream>
#include <string>
#include <unistd.h>

// --- Funções de Hardware (Preservadas) ---
double getRealRamUsage() {
    std::string line;
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) return 0.0;
    while (std::getline(statusFile, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::string valueStr = "";
            for (char c : line) if (std::isdigit(c)) valueStr += c;
            return valueStr.empty() ? 0.0 : std::stol(valueStr) / 1024.0;
        }
    }
    return 0.0;
}

int getRealCpuUsage() {
    double load[1];
    if (getloadavg(load, 1) != -1) {
        int cpu = static_cast<int>(load[0] * 10);
        return (cpu > 100) ? 99 : (cpu < 1 ? 1 : cpu);
    }
    return 1;
}

// --- Construtor Principal do Piper ---
BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent), m_isPrivate(false)
{
    if (!profile) profile = QWebEngineProfile::defaultProfile();
    
    // 1. Instalação da Sniper (UrlInterceptor)
    // O Interceptor cuida do bloqueio de rede (Camada 1)
    profile->setUrlRequestInterceptor(new UrlInterceptor(this));

    // 2. Configuração do Motor de Performance
    // Note: A extensão agora é carregada via flag no main.cpp para evitar erros de versão do Qt
    QWebEngineSettings *s = profile->settings();
    s->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    s->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    s->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    s->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, true);
    s->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);

    // --- Layout e UI ---
    setWindowTitle("Piper Browser - Sniper Edition");
    resize(1200, 800);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    topBar = new QWidget();
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background-color: #f1f3f4; border-bottom: 1px solid #ccc;");
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 0, 10, 0);

    backButton = new QPushButton("←");
    refreshButton = new QPushButton("↻");
    urlEdit = new QLineEdit();
    urlEdit->setPlaceholderText("Busque ou digite uma URL...");
    addTabButton = new QPushButton("+");
    homeButton = new QPushButton("🏠");
    historyButton = new QPushButton("📜");
    privateModeButton = new QPushButton("🕵");
    downloadButton = new QPushButton("📥");

    QString btnStyle = "QPushButton { border: none; font-size: 18px; padding: 5px; } QPushButton:hover { background-color: #ddd; }";
    backButton->setStyleSheet(btnStyle);
    refreshButton->setStyleSheet(btnStyle);
    addTabButton->setStyleSheet(btnStyle);
    homeButton->setStyleSheet(btnStyle);
    historyButton->setStyleSheet(btnStyle);
    privateModeButton->setStyleSheet(btnStyle);
    downloadButton->setStyleSheet(btnStyle);

    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(urlEdit);
    topLayout->addWidget(addTabButton);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(historyButton);
    topLayout->addWidget(privateModeButton);
    topLayout->addWidget(downloadButton);

    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    // Barra de Status (Medidores)
    QWidget *statusBar = new QWidget();
    statusBar->setFixedHeight(25);
    statusBar->setStyleSheet("background: #4169E1; color: white; font-size: 11px;");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(10, 0, 10, 0);
    QLabel *cpuLabel = new QLabel("CPU: 0%");
    QLabel *ramLabel = new QLabel("RAM: 0MB");
    statusLayout->addWidget(cpuLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(ramLabel);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);
    rootLayout->addWidget(statusBar);

    // Timer dos Medidores
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        cpuLabel->setText(QString("CPU: %1%").arg(getRealCpuUsage()));
        ramLabel->setText(QString("RAM: %1MB").arg(static_cast<int>(getRealRamUsage())));
    });
    timer->start(2000);

    // Conexões
    connect(urlEdit, &QLineEdit::returnPressed, [=]() {
        QString text = urlEdit->text();
        if(!text.startsWith("http")) text = "https://www.google.com/search?q=" + text;
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->load(QUrl(text));
    });
    connect(backButton, &QPushButton::clicked, [=]() { if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back(); });
    connect(refreshButton, &QPushButton::clicked, [=]() { if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload(); });
    connect(addTabButton, &QPushButton::clicked, [=]() { createNewTab(); });
    connect(tabWidget, &QTabWidget::tabCloseRequested, [=](int index) { if(tabWidget->count() > 1) tabWidget->removeTab(index); });
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(downloadButton, &QPushButton::clicked, this, &BrowserWindow::showDownloads);

    connect(profile, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    createNewTab(QUrl("about:blank"));
    goHome();
}

BrowserWindow::~BrowserWindow() {}

void BrowserWindow::createNewTab(const QUrl &url) {
    QWebEngineView *view = new QWebEngineView();
    view->load(url.isEmpty() ? QUrl("about:blank") : url);
    int index = tabWidget->addTab(view, "Nova Aba");
    tabWidget->setCurrentIndex(index);

    connect(view, &QWebEngineView::titleChanged, [=](const QString &title) {
        tabWidget->setTabText(tabWidget->indexOf(view), title.left(15));
    });
    connect(view, &QWebEngineView::urlChanged, [=](const QUrl &u) {
        if(tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if(!m_isPrivate) historyList.append(u.toString());
    });
}

// --- Gerenciamento de Downloads ---
void BrowserWindow::handleDownload(QWebEngineDownloadRequest *download) {
    QString path = QFileDialog::getSaveFileName(this, "Salvar Arquivo", download->downloadFileName());
    
    if (path.isEmpty()) {
        download->cancel();
        return;
    }

    QFileInfo fi(path);
    download->setDownloadDirectory(fi.absolutePath());
    download->setDownloadFileName(fi.fileName());
    
    download->accept();
    m_downloads.append(download);
}

void BrowserWindow::showDownloads() {
    QMenu *menu = new QMenu(this);
    if (m_downloads.isEmpty()) {
        menu->addAction("Nenhum download recente.");
    } else {
        for (int i = m_downloads.size() - 1; i >= 0; --i) {
            auto *d = m_downloads.at(i);
            QString status = (d->state() == QWebEngineDownloadRequest::DownloadInProgress) ? " (Baixando...)" : " (OK)";
            QAction *act = menu->addAction(d->downloadFileName() + status);
            connect(act, &QAction::triggered, [d]() { QDesktopServices::openUrl(QUrl::fromLocalFile(d->downloadDirectory())); });
        }
    }
    menu->exec(downloadButton->mapToGlobal(QPoint(0, downloadButton->height() + 5)));
}

void BrowserWindow::openPrivateWindow() {
    QWebEngineProfile *privateProfile = new QWebEngineProfile(this);
    BrowserWindow *privateWin = new BrowserWindow(privateProfile);
    privateWin->m_isPrivate = true;
    privateWin->setStyleSheet("QMainWindow { border: 3px solid #4B0082; }");
    privateWin->show();
}

void BrowserWindow::goHome() {
    if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget()))
        v->load(QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")));
}

void BrowserWindow::showHistory() {
    QString html = "<html><body style='background:#eef2ff; font-family:sans-serif; color:#4169E1; padding:20px;'>"
                   "<h1>Histórico Piper</h1>";
    for(int i = historyList.size()-1; i >= 0; --i) {
        html += QString("<div style='background:white; padding:10px; margin-bottom:5px; border-radius:4px;'>%1</div>").arg(historyList.at(i));
    }
    html += "</body></html>";
    
    QWebEngineView *historyView = new QWebEngineView();
    historyView->setHtml(html);
    tabWidget->addTab(historyView, "Histórico");
    tabWidget->setCurrentWidget(historyView);
}