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
    QStringList trackerBases;
    QStringList adBases;
    QStringList pathBlacklist;

    bool isTracker(const QString &urlStr, const QString &host);
    bool isAdReforço(const QString &urlStr, const QString &host);
    bool isBlacklistedPath(const QString &path);
    bool hasEnemySignature(const QString &urlStr);
};

#endif // URLINTERCEPTOR_H