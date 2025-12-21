#include "urlinterceptor.h"
#include <QDebug>
#include <algorithm> // Necessário para std::any_of (mais rápido que loops manuais)

int UrlInterceptor::globalAdsBlocked = 0;

UrlInterceptor::UrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
    // --- CAMADA 1: INFRAESTRUTURA GLOBAL E REDES SOCIAIS ---
    trackerBases << "analytics" << "hotjar" << "mouseflow" << "bugsnag" << "sentry" 
                 << "luckyorange" << "freshmarketer" << "facebook" << "tiktok" 
                 << "pinterest" << "linkedin" << "reddit" << "doubleclick" 
                 << "googlesyndication" << "googleadservices" << "adservice.google" 
                 << "adcolony" << "media.net" << "yahoo" << "yandex" << "unity3d" 
                 << "byteoversea" << "twitter" << "ads-api";

    // --- CAMADA 2: TELEMETRIA DE FABRICANTES E PERFORMANCE (K20) ---
    adBases << "xiaomi" << "miui" << "hicloud" << "oppomobile" << "realme" 
            << "samsung" << "apple" << "icloud" << "oneplus" << "2o7.net"
            << "nr-data.net" << "newrelic.com";

    // --- CAMADA 3: REDES PUBLICITÁRIAS, SSPs E CRITEO/MGID ---
    trackerBases << "criteo" << "crtd.com" << "mgid" << "rubiconproject" << "pubmatic" 
                 << "casalemedia" << "openx" << "outbrain" << "taboola" 
                 << "adnxs" << "scorecardresearch" << "smartadserver"
                 << "revcontent" << "adblade" << "triplelift" << "popads"
                 << "indexexchange" << "bat.bing" << "bingads" << "ads.microsoft"
                 << "clarity.ms" << "propellerads" << "applovin" << "vungle" 
                 << "liftoff.io" << "ironsource" << "supersonicads" << "inmobi" 
                 << "fyber" << "chartboost" << "doubleverify" << "adsafeprotected"
                 << "adsrvr.org" << "smartyads" << "ad.gt" << "contextweb" 
                 << "sharethrough" << "3lift.com" << "sonobi" << "gumgum" 
                 << "teads" << "kargo.com" << "adroll.com" << "nextroll.com"
                 << "insightexpressai.com" << "at-ra.com";

    // --- CAMADA 4: YOUTUBE E GOOGLE INFRA (ABATE DEFINITIVO DO REDIRECTOR) ---
    trackerBases << "s.youtube.com" << "youtubei.googleapis" << "youtube-ui" 
                 << "redirector.googlevideo.com" << "videostats.l.google.com"
                 << "play.google.com/log" << "googletagmanager.com" << "tagmanager.google.com";

    // --- CAMADA 5: ANÁLISE, FINGERPRINT E IDENTIDADE ---
    trackerBases << "mixpanel.com" << "amplitude.com" << "segment.com" << "segment.io"
                 << "fullstory.com" << "quantserve.com" << "quantcast.com"
                 << "fingerprintjs.com" << "fpjs.io" << "siftscience.com" << "sift.com"
                 << "permutive.com" << "onetag-sys.com" << "pippio.com" << "id5-sync.com"
                 << "thetradedesk.com" << "mathtag.com" << "tapad.com" << "prod.uidapi.com";

    // --- CAMADA 6: ATRIBUIÇÃO MÓVEL, LIVERAMP E PUSH (K19) ---
    trackerBases << "rlcdn.com" << "appsflyer.com" << "adjust.com" << "branch.io" 
                 << "bnc.lt" << "kochava.com" << "singular.net" << "wzrkt.com" 
                 << "clevertap-prod.com";

    // --- CAMADA 7: INFRAESTRUTURA DE APOIO E CDNs ---
    trackerBases << "amazon-adsystem" << "aan.amazon.com" << "amazonaws.com" 
                 << "googleusercontent" << "cloudfront.net";

    // --- CAMADA DE CAMINHOS (PATH BLACKLIST) ---
    pathBlacklist << "/ads/" << "/adv/" << "/metrics" << "/telemetry" << "/tracker"
                  << "/pixel" << "/collect" << "/log-event" << "/ad-delivery"
                  << "/adserver" << "/prebid" << "/amazon-adsystem" << "/google-ads"
                  << "/tr/" << "/p/" << "/v1/" << "/v2/" << "/mvt/" << "/sync"
                  << "/events/" << "/batch" << "/error-reporting" << "/delivery/"
                  << "/track/" << "/bid" << "/js-agent" << "/nr-ext" << "/gtm.js"
                  << "/ptracking" << "/user-sync" << "/identify" << "/fs.js" << "/fp.js";
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QString urlStr = info.requestUrl().toString().toLower();
    QString host = info.requestUrl().host().toLower();
    QString path = info.requestUrl().path().toLower();

    // --- FILTRO CIRÚRGICO: BLOQUEIO IMEDIATO DE ASSINATURAS CRÍTICAS ---
    if (host.contains("redirector.googlevideo.com") || 
        host.contains("bam.nr-data.net") || 
        urlStr.contains("adformat") || 
        urlStr.contains("adunit") ||
        urlStr.contains("&ad=") ||
        urlStr.contains("adroll_") ||
        urlStr.contains("gtm_") ||
        urlStr.contains("fingerprint") ||
        urlStr.contains("appsflyer")) {
        
        info.block(true);
        globalAdsBlocked++;
        return;
    }

    // --- VERIFICAÇÃO GLOBAL ---
    if (isTracker(urlStr, host) || isAdReforço(urlStr, host) || 
        isBlacklistedPath(path) || hasEnemySignature(urlStr)) {
        
        info.block(true);
        globalAdsBlocked++;
        
        qDebug() << ">>> PIPER SNIPER [ABATE]:" << host << "PATH:" << path;
    }
}

bool UrlInterceptor::isTracker(const QString &urlStr, const QString &host)
{
    return std::any_of(trackerBases.begin(), trackerBases.end(), [&](const QString &base) {
        return host.contains(base) || urlStr.contains(base);
    });
}

bool UrlInterceptor::isAdReforço(const QString &urlStr, const QString &host)
{
    return std::any_of(adBases.begin(), adBases.end(), [&](const QString &base) {
        return host.contains(base) || urlStr.contains(base);
    });
}

bool UrlInterceptor::isBlacklistedPath(const QString &path)
{
    if (path.length() < 3) return false;
    return std::any_of(pathBlacklist.begin(), pathBlacklist.end(), [&](const QString &p) {
        return path.contains(p);
    });
}

bool UrlInterceptor::hasEnemySignature(const QString &urlStr)
{
    static const QStringList signatures = {
        "ads.js", "pagead", "wp-ads", "smartad", "popunder", "ad-status",
        "detector.js", "blocker-detect", "advertisement", "ad-min.js", "prebid",
        "gtm.js", "fbevents.js", "analytics.js", "fbevents", "amazon-ad",
        "criteo", "mgid", "bingads", "appsflyer", "adjust", "newrelic", "nr-data",
        "fullstory", "quantserve", "mixpanel", "amplitude", "segment", "permutive"
    };

    for (const QString &sig : signatures) {
        if (urlStr.contains(sig)) return true;
    }

    // Bloqueio de Beacon (Rastreadores identificados pelo comprimento da URL)
    if ((urlStr.contains("tid=") || urlStr.contains("cid=") || urlStr.contains("uid=")) 
        && urlStr.length() > 65) {
        return true;
    }

    return false;
}