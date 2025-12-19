#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QWidget>
#include <QUrl>
#include <QDir>
#include <QIcon>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QWebEngineView>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStringLiteral>
#include <QDebug> 
#include <QTimer>
#include <fstream>
#include <string>
#include <unistd.h>

// --- Funções de Hardware Exclusivas do Processo Piper ---

double getRealRamUsage() {
    std::string line;
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) return 0.0;

    while (std::getline(statusFile, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::string valueStr = "";
            for (char c : line) {
                if (std::isdigit(c)) valueStr += c;
            }
            if (!valueStr.empty()) {
                return std::stol(valueStr) / 1024.0; // kB para MB
            }
        }
    }
    return 0.0;
}

int getRealCpuUsage() {
    double load[1];
    if (getloadavg(load, 1) != -1) {
        int cpu = static_cast<int>(load[0] * 10);
        if (cpu < 1) cpu = 1; 
        return (cpu > 100) ? 99 : cpu;
    }
    return 1;
}

BrowserWindow::BrowserWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- Configurações de Identidade ---
    setWindowTitle("Piper - browser");
    setWindowIcon(QIcon(QDir::current().absoluteFilePath("res/icons/piperos.png")));

    // --- 1. Layout Principal ---
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- 2. Barra Lateral de Controle ---
    QWidget *sidebarContainer = new QWidget();
    sidebarContainer->setFixedWidth(50);
    sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(0, 5, 0, 5);
    sidebarLayout->setSpacing(5);

    sidebarTabs = new QListWidget();
    sidebarTabs->setStyleSheet("QListWidget { border: none; background-color: #252526; }");
    sidebarLayout->addWidget(sidebarTabs);
    sidebarLayout->addStretch(1);

    // --- 3. Botões da Interface (BARRA LATERAL COMPLETA) ---
    backButton = new QPushButton();
    backButton->setFixedSize(40, 40);
    backButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/voltar.png")));
    backButton->setIconSize(QSize(32, 32));
    backButton->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 5px; } QPushButton:hover { background: #3c3c3c; }");

    refreshButton = new QPushButton();
    refreshButton->setFixedSize(40, 40);
    refreshButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/recarregar.png")));
    refreshButton->setIconSize(QSize(32, 32));
    refreshButton->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 5px; } QPushButton:hover { background: #3c3c3c; }");

    historyButton = new QPushButton();
    historyButton->setFixedSize(40, 40);
    historyButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/history.png")));
    historyButton->setIconSize(QSize(32, 32));
    historyButton->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 5px; } QPushButton:hover { background: #3c3c3c; }");

    homeButton = new QPushButton();
    homeButton->setFixedSize(40, 40);
    homeButton->setIcon(QIcon(QDir::current().absoluteFilePath("res/icons/home.png")));
    homeButton->setIconSize(QSize(32, 32));
    homeButton->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 5px; } QPushButton:hover { background: #3c3c3c; }");

    sidebarLayout->addWidget(backButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(refreshButton, 0, Qt::AlignCenter);
    sidebarLayout->addWidget(historyButton, 0, Qt::AlignCenter); // Histórico Restaurado
    sidebarLayout->addWidget(homeButton, 0, Qt::AlignCenter);

    // --- 4. Motor do Escudo e Perfil ---
    QWebEngineProfile *profile = QWebEngineProfile::defaultProfile();
    QString ua = QStringLiteral("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36");
    profile->setHttpUserAgent(ua);
    profile->setPersistentStoragePath(QDir::currentPath() + "/storage");

    UrlInterceptor *interceptor = new UrlInterceptor(this);
    profile->setUrlRequestInterceptor(interceptor);

    // --- 5. WebView e Configurações ---
    webView = new QWebEngineView(this);
    webView->page()->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    webView->page()->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);

    // --- 6. Conexões e Sinais ---
    connect(backButton, &QPushButton::clicked, webView, &QWebEngineView::back);
    connect(refreshButton, &QPushButton::clicked, webView, &QWebEngineView::reload);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);

    connect(webView, &QWebEngineView::urlChanged, this, [this](const QUrl &url) {
        QString urlStr = url.toString();
        QString homePath = QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")).toString();
        if (urlStr != homePath && !urlStr.isEmpty() && !urlStr.startsWith("data:text/html")) {
            if (historyList.isEmpty() || historyList.last() != urlStr) {
                historyList.append(urlStr);
                if (historyList.size() > 100) historyList.removeFirst();
            }
        }
    });

    // Limpeza Cosmética (Remove anúncios visuais)
    connect(webView, &QWebEngineView::loadFinished, this, [this](bool ok) {
        if (ok) {
            QString clean = "['.ad-slot', '.adsbygoogle', '#ad-container', '.ad-unit'].forEach(s => "
                            "document.querySelectorAll(s).forEach(el => el.remove()));";
            webView->page()->runJavaScript(clean);
        }
    });

    // --- 7. Lógica do Dashboard (Telemetria Real) ---
    QTimer *statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, [this]() {
        QString url = webView->url().toString();
        if (url.contains("homepage.html") || url.isEmpty() || url.startsWith("file://")) {
            int ads = UrlInterceptor::globalAdsBlocked;
            double data = ads * 0.15; 
            int cpu = getRealCpuUsage();
            double ram = getRealRamUsage(); 

            QString js = QString("if(typeof updatePiperStats === 'function') { "
                                 "updatePiperStats(%1, %2, %3, %4); }")
                                 .arg(ads).arg(data, 0, 'f', 1).arg(cpu).arg(ram, 0, 'f', 1);
            
            webView->page()->runJavaScript(js);
        }
    });
    statsTimer->start(1500); 

    mainLayout->addWidget(sidebarContainer);
    mainLayout->addWidget(webView);

    goHome();
    showMaximized();
}

void BrowserWindow::showHistory() {
    QString html = "<html><head><style>"
                   "body { background-color: #1e1e1e; color: white; font-family: sans-serif; padding: 30px; }"
                   "h1 { color: #58a6ff; border-bottom: 1px solid #30363d; padding-bottom: 10px; }"
                   ".item { background: #21262d; padding: 10px; border-radius: 6px; margin-bottom: 8px; border: 1px solid #30363d; cursor: pointer; }"
                   ".item:hover { background: #30363d; }"
                   ".url { color: #8b949e; font-size: 12px; word-break: break-all; }"
                   "</style></head><body>"
                   "<h1>Histórico de Navegação</h1>";

    if (historyList.isEmpty()) {
        html += "<p>Nenhum site visitado ainda.</p>";
    } else {
        for (int i = historyList.size() - 1; i >= 0; --i) {
            QString url = historyList.at(i);
            html += QString("<div class='item' onclick='window.location=\"%1\"'>"
                            "<div>Site Visitado</div>"
                            "<div class='url'>%1</div>"
                            "</div>").arg(url);
        }
    }
    html += "</body></html>";
    webView->setHtml(html);
}

void BrowserWindow::goHome() {
    QString path = QDir::current().absoluteFilePath("homepage.html");
    webView->load(QUrl::fromLocalFile(QDir::cleanPath(path)));
}

BrowserWindow::~BrowserWindow() {}