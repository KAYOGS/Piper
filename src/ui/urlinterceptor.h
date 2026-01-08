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
    QSet<QString> adHosts;
    QStringList adPatterns;
    QStringList blacklistedPaths;

    void initializeLists();
    void applyGlobalOptimizations(QWebEngineUrlRequestInfo &info);
};

#endif // URLINTERCEPTOR_H
