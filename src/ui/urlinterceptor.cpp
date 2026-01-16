#include "urlinterceptor.h"
#include <QDebug>

int UrlInterceptor::globalAdsBlocked = 0;

UrlInterceptor::UrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
    initializeLists();
}

void UrlInterceptor::initializeLists()
{
    adHosts << "doubleclick.net" << "googleadservices.com" << "googlesyndication.com"
            << "adnxs.com" << "adsrvr.org" << "taboola.com" << "outbrain.com"
            << "popads.net" << "casalemedia.com" << "pubmatic.com" << "criteo.com"
            << "mgid.com" << "scorecardresearch.com" << "quantserve.com"
            << "advertising.com" << "yieldmo.com" << "rubiconproject.com"
            << "adform.net" << "bidswitch.net" << "smartadserver.com"
            << "aniview.com" << "openx.net" << "pixel.facebook.com"
            << "analytics.google.com" << "google-analytics.com" << "hotjar.com"
            << "fonts.googleapis.com" << "fonts.gstatic.com" << "use.typekit.net";

    adPatterns << "/ads." << "/ads/" << "ads.js" << "advertisement" 
               << "telemetry" << "track.js" << "analytics.js"
               << "pixel." << "prebid" << "amazon-adsystem"
               << "fbevents.js" << "gtm.js" << "clarity.ms";

    blacklistedPaths << "/wp-content/plugins/ad-rotator/" 
                     << "/vpaid/" << "/vast-xml/" << "/ad-delivery/";
}

void UrlInterceptor::applyGlobalOptimizations(QWebEngineUrlRequestInfo &info)
{
    // User Agent do Firefox para máxima compatibilidade
    QByteArray firefox_UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:120.0) Gecko/20100101 Firefox/120.0";
    info.setHttpHeader("User-Agent", firefox_UA);

    info.setHttpHeader("Save-Data", "on");
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QUrl url = info.requestUrl();
    QString host = url.host().toLower();
    QString urlStr = url.toString().toLower();
    QString path = url.path().toLower();

    applyGlobalOptimizations(info);

    if (host.contains("google.com/recaptcha") || host.contains("gstatic.com/recaptcha")) {
        return;
    }

    bool block = false;
    
    for (const QString &adHost : adHosts) {
        if (host == adHost || host.endsWith("." + adHost)) {
            block = true;
            break;
        }
    }

    if (!block) {
        for (const QString &pattern : adPatterns) {
            if (urlStr.contains(pattern)) {
                block = true;
                break;
            }
        }
    }

    if (block) {
        info.block(true);
        globalAdsBlocked++;
        qDebug() << "[PIPER OPTIMIZER] Bloqueado (Recurso Pesado):" << host;
    }
}
