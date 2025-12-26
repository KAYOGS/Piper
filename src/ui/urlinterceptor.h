#ifndef URLINTERCEPTOR_H
#define URLINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QStringList>
#include <QUrl>
#include <QSet>

class UrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit UrlInterceptor(QObject *parent = nullptr);
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    static int globalAdsBlocked;

private:
    QSet<QString> adHosts;       // Domínios exatos de anúncios
    QStringList adPatterns;      // Padrões de URL de rastreio
    QStringList blacklistedPaths; // Caminhos suspeitos

    void initializeLists();
    bool shouldBlock(const QUrl &url);
};

#endif // URLINTERCEPTOR_H