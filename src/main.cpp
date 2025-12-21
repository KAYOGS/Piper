#include <QApplication>
#include <QWebEngineProfile>
#include <QIcon>
#include <QDir>
#include <QDebug>
#include "ui/browserwindow.h"

int main(int argc, char *argv[])
{
    // 1. Definimos o caminho onde o uBlock está guardado
    QString extensionPath = QDir::homePath() + "/Piper/extensions/ublock";
    
    // 2. Verificação de segurança: O Piper avisa no terminal se achou a pasta
    if (QFile::exists(extensionPath + "/manifest.json")) {
        qDebug() << "PIPER: Aliado uBlock localizado com sucesso!";
    } else {
        qWarning() << "PIPER: ERRO - Pasta uBlock não encontrada em:" << extensionPath;
    }

    // 3. Configuramos as ordens para o motor do Chromium
    // Ativamos extensões e pedimos para carregar a pasta do uBlock
    QString chromeFlags = "--enable-extensions --no-sandbox --load-extension=" + extensionPath;
    
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", chromeFlags.toUtf8());
    qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");

    QApplication a(argc, argv);

    // 4. Identidade do Navegador
    QApplication::setWindowIcon(QIcon("res/icones/piperos.png"));
    a.setDesktopFileName("piper");
    a.setAttribute(Qt::AA_DontUseNativeMenuBar);

    // 5. Iniciamos o Perfil e a Janela
    QWebEngineProfile *profile = QWebEngineProfile::defaultProfile();
    BrowserWindow w(profile);
    w.show();

    return a.exec();
}