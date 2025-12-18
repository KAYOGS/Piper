#ifndef URLINTERCEPTOR_H
#define URLINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlRequestInfo>
#include <QStringList>
#include <QUrl>

class UrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit UrlInterceptor(QObject *parent = nullptr);

    // O "Cérebro" do bloqueador: este método analisa cada pedido de rede
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

private:
    // Nossa lista de termos proibidos (Blacklist)
    QStringList adDomains;
    
    // Função auxiliar para verificar se a URL é suspeita
    bool isAd(const QUrl &url);
};

#endif // URLINTERCEPTOR_H