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
            if (!valueStr.empty()) return std::stol(valueStr) / 1024.0;
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
    
    setWindowTitle(m_isPrivate ? "Piper - Sessão Privada" : "Piper Professional");
    setWindowIcon(QIcon(QDir::current().absoluteFilePath("res/icons/piperos.png")));

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- BARRA SUPERIOR (PRETO RÚSTICO - 30px) ---
    topBar = new QWidget();
    topBar->setFixedHeight(30);
    topBar->setStyleSheet(m_isPrivate ? "background-color: #0d0d0d;" : "background-color: #121212;");
    
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(5, 0, 5, 0);
    topLayout->setSpacing(6);

    // Estilo para botões com ícones brancos
    QString btnStyle = "QPushButton { background: transparent; border: none; border-radius: 4px; } "
                       "QPushButton:hover { background: #2a2a2a; }";

    auto setupBtn = [&](QPushButton* &btn, QString iconPath, QString tip) {
        btn = new QPushButton();
        btn->setFixedSize(25, 25);
        btn->setIcon(QIcon(QDir::current().absoluteFilePath(iconPath)));
        btn->setIconSize(QSize(16, 16));
        btn->setToolTip(tip);
        btn->setStyleSheet(btnStyle);
    };

    // 1. Esquerda: Voltar e Recarregar
    setupBtn(backButton, "res/icons/voltar.png", "Voltar");
    setupBtn(refreshButton, "res/icons/recarregar.png", "Recarregar");

    // 2. Centro: Barra de Endereço (Tamanho Fixo: 280px para equilíbrio)
    urlEdit = new QLineEdit();
    urlEdit->setFixedWidth(280);
    urlEdit->setFixedHeight(22);
    urlEdit->setPlaceholderText("Pesquisar ou URL...");
    urlEdit->setStyleSheet("QLineEdit { background: #1e1e1e; color: white; border: 1px solid #333; border-radius: 4px; padding-left: 8px; font-size: 11px; }");

    // 3. Botão Nova Aba (+)
    setupBtn(addTabButton, "res/icons/novaaba.png", "Nova Aba");

    // 4. Container de Abas (Ocupa o centro)
    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: none; } "
        "QTabBar::tab { background: #121212; color: #8b949e; padding: 4px 10px; min-width: 80px; max-width: 150px; font-size: 10px; border: none; } "
        "QTabBar::tab:selected { background: #1e1e1e; color: white; border-bottom: 2px solid #58a6ff; } "
        "QTabBar::close-button { image: url(res/icons/fecharaba.png); } "
    );

    // 5. Direita: Funções Adicionais
    setupBtn(homeButton, "res/icons/home.png", "Início");
    setupBtn(historyButton, "res/icons/history.png", "Histórico");
    setupBtn(privateModeButton, "res/icons/incognito.png", "Privado");

    // Montagem do Layout na Ordem Solicitada
    topLayout->addWidget(backButton);
    topLayout->addWidget(refreshButton);
    topLayout->addWidget(urlEdit);
    topLayout->addWidget(addTabButton);
    topLayout->addWidget(tabWidget, 1); // Estica para ocupar o resto
    topLayout->addWidget(homeButton);
    topLayout->addWidget(historyButton);
    topLayout->addWidget(privateModeButton);

    // --- LÓGICA DE ABAS E EVENTOS ---

    connect(addTabButton, &QPushButton::clicked, this, [this]() { createNewTab(QUrl()); });

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
        QWidget *w = tabWidget->widget(index);
        tabWidget->removeTab(index);
        w->deleteLater(); // Encerra o processo da aba
        if (tabWidget->count() == 0) createNewTab(QUrl());
    });

    connect(urlEdit, &QLineEdit::returnPressed, this, [this]() {
        QString url = urlEdit->text().trimmed();
        if (url.isEmpty()) return;
        if (!url.contains(".")) url = "https://www.google.com/search?q=" + url;
        else if (!url.startsWith("http")) url = "https://" + url;
        
        if (auto *view = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
            view->load(QUrl(url));
        }
    });

    connect(backButton, &QPushButton::clicked, this, [this]() {
        if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->back();
    });
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->reload();
    });

    connect(homeButton, &QPushButton::clicked, this, &BrowserWindow::goHome);
    connect(historyButton, &QPushButton::clicked, this, &BrowserWindow::showHistory);
    connect(privateModeButton, &QPushButton::clicked, this, &BrowserWindow::openPrivateWindow);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(tabWidget);

    // Inicia com a homepage
    createNewTab(QUrl());
    showMaximized();

    // Timer de Stats para a Homepage
    QTimer *statsTimer = new QTimer(this);
    connect(statsTimer, &QTimer::timeout, this, [this]() {
        if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
            if (v->url().toString().contains("homepage.html")) {
                QString js = QString("if(typeof updatePiperStats === 'function') { updatePiperStats(%1, %2, %3, %4); }")
                    .arg(UrlInterceptor::globalAdsBlocked)
                    .arg(UrlInterceptor::globalAdsBlocked * 0.15, 0, 'f', 1)
                    .arg(getRealCpuUsage()).arg(getRealRamUsage(), 0, 'f', 1);
                v->page()->runJavaScript(js);
            }
        }
    });
    statsTimer->start(1500);
}

void BrowserWindow::createNewTab(const QUrl &url) {
    QWebEngineView *view = new QWebEngineView();
    QWebEngineProfile *profile = m_isPrivate ? new QWebEngineProfile(view) : QWebEngineProfile::defaultProfile();
    
    QWebEnginePage *page = new QWebEnginePage(profile, view);
    view->setPage(page);

    int index = tabWidget->addTab(view, QIcon(QDir::current().absoluteFilePath("res/icons/piperos.png")), "Carregando...");
    tabWidget->setCurrentIndex(index);

    connect(view, &QWebEngineView::titleChanged, this, [this, view](const QString &title) {
        int idx = tabWidget->indexOf(view);
        tabWidget->setTabText(idx, title.isEmpty() ? "Nova Aba" : title.left(12) + "...");
    });

    connect(view, &QWebEngineView::iconChanged, this, [this, view](const QIcon &icon) {
        tabWidget->setTabIcon(tabWidget->indexOf(view), icon);
    });

    connect(view, &QWebEngineView::urlChanged, this, [this, view](const QUrl &u) {
        if (tabWidget->currentWidget() == view) urlEdit->setText(u.toString());
        if (!m_isPrivate && !u.toString().contains("homepage.html")) historyList.append(u.toString());
    });

    if (url.isEmpty()) goHome();
    else view->load(url);
}

void BrowserWindow::goHome() {
    if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) {
        v->load(QUrl::fromLocalFile(QDir::current().absoluteFilePath("homepage.html")));
    }
}

void BrowserWindow::openPrivateWindow() {
    QWebEngineProfile *priv = new QWebEngineProfile();
    BrowserWindow *win = new BrowserWindow(priv);
    win->setAttribute(Qt::WA_DeleteOnClose);
    win->show();
}

void BrowserWindow::showHistory() {
    QString html = "<html><head><style>body{background:#1a1b1e;color:#e0e0e0;font-family:sans-serif;padding:20px;} .item{background:#21262d;padding:10px;margin-bottom:5px;border-radius:4px;cursor:pointer;} .item:hover{background:#30363d;}</style></head><body><h1>Histórico</h1>";
    for(int i=historyList.size()-1; i>=0; --i) html += QString("<div class='item' onclick='window.location=\"%1\"'>%1</div>").arg(historyList.at(i));
    html += "</body></html>";
    if (auto *v = qobject_cast<QWebEngineView*>(tabWidget->currentWidget())) v->setHtml(html);
}

void BrowserWindow::handleDownload(QWebEngineDownloadRequest *download) {
    QString path = QFileDialog::getSaveFileName(this, "Salvar", QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + download->downloadFileName());
    if (path.isEmpty()) { download->cancel(); return; }
    download->setDownloadDirectory(QFileInfo(path).absolutePath());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
}

BrowserWindow::~BrowserWindow() {}