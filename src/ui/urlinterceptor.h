#ifndef URLINTERCEPTOR_H
#define URLINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QStringList>

class UrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit UrlInterceptor(QObject *parent = nullptr);

    // Método principal que decide o destino de cada requisição
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    // Contador global para alimentar o dashboard do PiperHub
    static int globalAdsBlocked;

private:
    // Lista de domínios inimigos da performance
    QStringList adDomains;

    // Função interna para checar se a URL é lixo publicitário
    bool isAd(const QUrl &url);
};

#endif // URLINTERCEPTOR_H