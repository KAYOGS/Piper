#include "browserwindow.h"
#include "urlinterceptor.h"
#include <QDir>
#include <QIcon>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QWebEngineDownloadRequest>
#include <QFileInfo>
#include <fstream>
#include <string>
#include <unistd.h>

// --- Funções de Hardware ---
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

BrowserWindow::BrowserWindow(QWebEngineProfile *profile, QWidget *parent)
    : QMainWindow(parent)
{
    m_isPrivate = (profile && profile->isOffTheRecord());
    setWindowTitle(m_isPrivate ? "Piper - Navegação Privada" : "Piper Professional");
    setWindowIcon(QIcon("res/icons/piperos.png"));

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- BARRA INTEGRADA PREMIUM (45px para melhor respiro) ---
    topBar = new QWidget();
    topBar->setFixedHeight(45);
    topBar->setStyleSheet("background-color: #4169E1; border-bottom: 1px solid #27408B;");
    
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);
    topLayout->setSpacing(8);

    // Lambda para criação de botões uniformes e elegantes
    auto setupBtn = [&](QPushButton* &btn, QString iconPath, QString tip) {
        btn = new QPushButton();
        btn->setFixedSize(32, 32);
        btn->setIcon(QIcon(QDir::current().absoluteFilePath(iconPath)));
        btn->setIconSize(QSize(20, 20));
        btn->setToolTip(tip);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background: transparent; border-radius: 6px; border: none; } "
            "QPushButton:hover { background: rgba(255, 255, 255, 0.2); } "
            "QPushButton:pressed { background: rgba(0, 0, 0, 0.1); }"
        );
    };

    setupBtn(backButton, "res/icons/voltar.png", "Voltar");
    setupBtn(refreshButton, "res/icons/recarregar.png", "Recarregar");

    // Barra de URL: Flexível e com design moderno arredondado
    urlEdit = new QLineEdit();
    urlEdit->setFixedHeight(32);
    urlEdit->setPlaceholderText("Pesquisar no Piper...");
    urlEdit->setStyleSheet(
        "QLineEdit { "
        "  background: rgba(255, 255, 255, 0.95); "
        "  color: #333; "
        "  border: 1px solid #27408B; "
        "  border-radius: 16px; "
        "  padding: 0 15px; "
        "  font-size: 13px; "
        "} "
        "QLineEdit:focus { border: 2px solid #fff; background: #fff; }"
    );

    setupBtn(addTabButton, "res/icons/novaaba.png", "Nova Aba");

    // Inicialização do TabWidget com estilo refinado
    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; } "
        "QTabBar::tab { "
        "  background: rgba(0, 0, 0, 0.15); "
        "  color: white; "
        "  padding: 6px 15px; "
        "  margin-right: 4px; "
        "  border-top-left-radius: 8px; "
        "  border-top-right-radius: 8px; "
        "  min-width: 140px; "
        "} "
        "QTabBar::tab:selected { "
        "  background: #f0f0f0; "
        "  color: #4169E1; "
        "  font-weight: bold; "
        "} "
        "QTabBar::tab:hover:!selected { background: rgba(255, 255, 255, 0.1); } "
        "QTabBar::close-button { image: url(res/icons/fecharaba.png); subcontrol-position: right; } "
    );

    setupBtn(homeButton, "res/icons/home.png", "Início");
    setupBtn(historyButton, "res/icons/history.png", "Histórico");
    setupBtn(privateModeButton, "res/icons/incognito.png", "Privado");

    // Distribuição do Layout Superior (Proporções: URL 2x, Abas 3x)
    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(urlEdit, 2);      
    topLayout->addSpacing(5);
    topLayout->addWidget(tabWidget, 3);    
    topLayout->addWidget(addTabButton);
    topLayout->addSpacing(10);
    topLayout->addWidget(homeButton);
    topLayout->addWidget(historyButton);
    topLayout->addWidget(privateModeButton);

    // Conexões de sinais
    connect(addTabButton, &QPushButton::clicked, this, [this](){ createNewTab(); });
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index){
        QWidget *w = tabWidget->widget(index);
        tabWidget->removeTab(index);
        w->deleteLater(); 
        if(tabWidget->count() == 0) createNewTab();
    });

    connect(urlEdit, &QLineEdit::returnPressed, this, [this](){
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
            QString url = urlEdit->text().trimmed();
            if(!url.contains(".")) url = "https://www.google.com/search?q=" + url;
            else if(!url.startsWith("http")) url = "https://" + url;
            v->load(QUrl(url));
        }
    });

    connect(backButton, &QPushButton::clicked, this, [this](){ if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back(); });
    connect(refreshButton, &QPushButton::clicked, this, [this](){ if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload(); });
    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);

    createNewTab();
    showMaximized();

    // Timer de atualização de Hardware (stats na homepage)
    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [this](){
        if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
            if(v->url().toString().contains("homepage.html")) {
                v->page()->runJavaScript(QString("if(typeof updatePiperStats === 'function') updatePiperStats(%1, %2, %3, %4);")
                    .arg(UrlInterceptor::globalAdsBlocked).arg(UrlInterceptor::globalAdsBlocked * 0.15)
                    .arg(getRealCpuUsage()).arg(getRealRamUsage()));
            }
        }
    });
    t->start(1500);
}

void BrowserWindow::createNewTab(const QUrl &url) {
    QWebEngineView *view = new QWebEngineView();
    QWebEngineProfile *prof = m_isPrivate ? new QWebEngineProfile(view) : QWebEngineProfile::defaultProfile();
    view->setPage(new QWebEnginePage(prof, view));

    int idx = tabWidget->addTab(view, QIcon("res/icons/piperos.png"), "Piper");
    tabWidget->setCurrentIndex(idx);

    connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString &t){
        int i = tabWidget->indexOf(view);
        if(i != -1) tabWidget->setTabText(i, t.left(15)); // Aumentado limite de caracteres do título
    });

    connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl &u){
        if(tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if(!m_isPrivate && !u.toString().contains("homepage.html")) historyList.append(u.toString());
    });

    connect(prof, &QWebEngineProfile::downloadRequested, this, &BrowserWindow::handleDownload);

    if(url.isEmpty()) goHome();
    else view->load(url);
}

void BrowserWindow::handleDownload(QWebEngineDownloadRequest *download) {
    QString path = QFileDialog::getSaveFileName(this, "Salvar Arquivo", 
                   QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + download->downloadFileName());
    
    if (path.isEmpty()) {
        download->cancel();
        return;
    }

    download->setDownloadDirectory(QFileInfo(path).absolutePath());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
}

void BrowserWindow::goHome() {
    if(auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        v->load(QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")));
    }
}

void BrowserWindow::showHistory() {
    QString html = "<html><body style='background:#eef2ff; font-family:sans-serif; color:#4169E1; padding:20px;'>"
                   "<h1>Histórico Piper</h1>";
    for(int i = historyList.size()-1; i >= 0; --i) {
        html += QString("<p style='background:white; padding:10px; border-radius:5px;'>%1</p>").arg(historyList.at(i));
    }
    html += "</body></html>";
    if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->setHtml(html);
}

void BrowserWindow::openPrivateWindow() {
    BrowserWindow *win = new BrowserWindow(new QWebEngineProfile());
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

BrowserWindow::~BrowserWindow() {}