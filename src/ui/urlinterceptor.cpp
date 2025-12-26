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
    // --- DOMÍNIOS DE ANÚNCIOS E RASTREIO (BLOCKLIST CIRÚRGICA) ---
    // Usamos QSet para busca rápida O(1)
    adHosts << "doubleclick.net" << "googleadservices.com" << "googlesyndication.com"
            << "adnxs.com" << "adsrvr.org" << "taboola.com" << "outbrain.com"
            << "popads.net" << "casalemedia.com" << "pubmatic.com" << "criteo.com"
            << "mgid.com" << "scorecardresearch.com" << "quantserve.com"
            << "advertising.com" << "yieldmo.com" << "rubiconproject.com"
            << "adform.net" << "bidswitch.net" << "smartadserver.com"
            << "aniview.com" << "openx.net" << "pixel.facebook.com"
            << "analytics.google.com" << "google-analytics.com" << "hotjar.com";

    // --- PADRÕES DE URL (ASSINATURAS DE SCRIPTS) ---
    adPatterns << "/ads." << "/ads/" << "ads.js" << "advertisement" 
               << "telemetry" << "track.js" << "analytics.js"
               << "pixel." << "prebid" << "amazon-adsystem"
               << "fbevents.js" << "gtm.js" << "clarity.ms";

    // --- CAMINHOS ESPECÍFICOS ---
    blacklistedPaths << "/wp-content/plugins/ad-rotator/" 
                     << "/vpaid/" << "/vast-xml/" << "/ad-delivery/";
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QUrl url = info.requestUrl();
    QString host = url.host().toLower();
    QString urlStr = url.toString().toLower();
    QString path = url.path().toLower();

    // 1. Verificar se o Host está na lista de bloqueio direto
    bool block = false;
    
    // Verifica domínios e subdomínios (ex: ads.doubleclick.net)
    for (const QString &adHost : adHosts) {
        if (host == adHost || host.endsWith("." + adHost)) {
            block = true;
            break;
        }
    }

    // 2. Verificar padrões de URL se ainda não foi bloqueado
    if (!block) {
        for (const QString &pattern : adPatterns) {
            if (urlStr.contains(pattern)) {
                block = true;
                break;
            }
        }
    }

    // 3. Verificar caminhos proibidos
    if (!block) {
        for (const QString &badPath : blacklistedPaths) {
            if (path.startsWith(badPath)) {
                block = true;
                break;
            }
        }
    }

    // --- EXCEÇÕES DE SEGURANÇA (WHITELIST) ---
    // Nunca bloqueia o carregamento de vídeos do YouTube ou scripts vitais do Google
    if (host.contains("googlevideo.com") || host.contains("fonts.googleapis.com")) {
        block = false;
    }

    if (block) {
        info.block(true);
        globalAdsBlocked++;
        qDebug() << "[PIPER SNIPER] Bloqueado:" << host;
    }
}