#include "urlinterceptor.h"
#include <QDebug>

UrlInterceptor::UrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
    // --- BLACKLIST DE ELITE DO PIPER ---
    // Focada em domínios que servem anúncios e telemetria pesada
    adDomains << "google-analytics.com" 
              << "doubleclick.net" 
              << "googlesyndication.com"
              << "googleads.g.doubleclick.net"
              << "static.doubleclick.net"
              << "adservice.google.com"
              << "youtube.com/api/stats/ads" // Bloqueia chamadas de anúncio no YT
              << "adnxs.com" 
              << "advertising.com"
              << "adserver"
              << "analytics"
              << "telemetry"
              << "metrics"
              << "/ads/"
              << "ads.js"
              << "pixel.facebook.com"
              << "scorecardresearch.com"
              << "hotjar.com"
              << "popads.net"  // Comum em sites de anime
              << "popcash.net" // Comum em sites de anime
              << "exoclick.com"
              << "clicmanager.fr"
              << "moatads.com";
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    // Pegamos a URL solicitada
    QUrl url = info.requestUrl();
    QString urlString = url.toString().toLower();

    // Verificação ultra-rápida por padrões
    if (isAd(url)) {
        info.block(true); // Cancela o pedido antes de usar internet ou RAM
        qDebug() << "Piper Escudo [BLOQUEADO]:" << url.host();
    }
}

bool UrlInterceptor::isAd(const QUrl &url)
{
    QString urlString = url.toString().toLower();

    // Otimização: Se a URL contém termos óbvios de anúncios, bloqueia na hora
    for (const QString &pattern : adDomains) {
        if (urlString.contains(pattern)) {
            return true;
        }
    }

    return false;
}