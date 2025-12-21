#include "urlinterceptor.h"
#include <QDebug>
#include <algorithm> // Necessário para std::any_of (performance máxima)

int UrlInterceptor::globalAdsBlocked = 0;

UrlInterceptor::UrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
{
    // --- CAMADA 1: INFRAESTRUTURA GLOBAL E REDES SOCIAIS (X3, X4, X5) ---
    trackerBases << "analytics" << "hotjar" << "mouseflow" << "bugsnag" << "sentry" 
                 << "luckyorange" << "freshmarketer" << "facebook" << "tiktok" 
                 << "pinterest" << "linkedin" << "reddit" << "doubleclick" 
                 << "googlesyndication" << "googleadservices" << "adservice.google" 
                 << "adcolony" << "media.net" << "yahoo" << "yandex" << "unity3d" 
                 << "byteoversea" << "twitter" << "ads-api" << "instagram.com"
                 << "snapchat.com" << "sc-static.net" << "tumblr.com" << "vk.com" 
                 << "mail.ru" << "licdn.com" << "quora.com" << "appspot.com";

    // --- CAMADA 2: TELEMETRIA, HARDWARE E FABRICANTES (HUAWEI + AMAZON + APPLE + X6) ---
    adBases << "huawei.com" << "dbankcloud.com" << "xiaomi" << "miui" << "hicloud" 
            << "oppomobile" << "realme" << "samsung" << "apple" << "icloud" << "oneplus" 
            << "2o7.net" << "nr-data.net" << "newrelic.com" << "bam.nr-data.net"
            << "lge.com" << "lgtvcommon.com" << "smartclip.net" << "vortex.data.microsoft" 
            << "watson.telemetry.microsoft" << "device-metrics-us.amazon" << "device-metrics-us-2.amazon"
            << "mads-eu.amazon" << "logs.roku.com" << "ads.roku.com" << "mzstatic.com"
            << "vizio.com" << "app-measurement.com" << "firebase-settings";

    // --- CAMADA 3: VÍDEO ADS, VAST/VPAID E PLAYERS (X15) ---
    trackerBases << "imasdk.googleapis.com" << "dai.google.com" << "jwpsrv.com" 
                 << "jwpcdn.com" << "ssl.p.jwpcdn.com" << "fwrmm.net" << "mssl.fwrmm.net" 
                 << "connatix.com" << "cd.connatix.com" << "capi.connatix.com" 
                 << "vid.connatix.com" << "brightcove.com" << "metrics.brightcove.com" 
                 << "innovid.com" << "s.innovid.com" << "tremorhub.com" << "ads.tremorhub.com" 
                 << "springserve.com" << "spotxchange.com" << "vungle.com" << "adcolony.com";

    // --- CAMADA 4: REDES DE AFILIADOS E PERFORMANCE (X12) ---
    trackerBases << "anrdoezrs.net" << "dpbolvw.net" << "tkqlhce.com" << "shareasale.com" 
                 << "linksynergy.com" << "click.linksynergy.com" << "ad.linksynergy.com" 
                 << "track.linksynergy.com" << "impact.com" << "api.impact.com" 
                 << "d.impactradius-event.com" << "awin1.com" << "zenaps.com" 
                 << "partnerstack.com" << "referion.com" << "skimresources.com" 
                 << "viglink.com" << "sovrn.com" << "redirect.viglink.com";

    // --- CAMADA 5: GESTÃO DE CONSENTIMENTO E COOKIE BANNERS (X9) ---
    trackerBases << "cookielaw.org" << "cdn.cookielaw.org" << "onetrust.com" 
                 << "geolocation.onetrust.com" << "cookiebot.com" << "consent.cookiebot.com" 
                 << "trustarc.com" << "consent.trustarc.com" << "privacy-center.org" 
                 << "privacy-mgmt.com" << "usercentrics.eu" << "app.usercentrics.eu" 
                 << "quantcast.mgr.consensu.org" << "didomi.io";

    // --- CAMADA 6: REDES PUBLICITÁRIAS E NATIVE ADS (K5-K10) ---
    trackerBases << "criteo" << "crtd.com" << "mgid" << "rubiconproject" << "pubmatic" 
                 << "casalemedia" << "openx" << "outbrain" << "taboola" 
                 << "adnxs" << "scorecardresearch" << "smartadserver"
                 << "revcontent" << "adblade" << "triplelift" << "popads"
                 << "indexexchange" << "bat.bing" << "bingads" << "ads.microsoft"
                 << "clarity.ms" << "propellerads" << "applovin" << "liftoff.io" 
                 << "ironsource" << "supersonicads" << "inmobi" << "fyber" 
                 << "chartboost" << "doubleverify" << "adsafeprotected"
                 << "adsrvr.org" << "smartyads" << "ad.gt" << "contextweb" 
                 << "sharethrough" << "3lift.com" << "sonobi" << "gumgum" 
                 << "teads" << "kargo.com" << "adroll.com" << "nextroll.com"
                 << "insightexpressai.com" << "at-ra.com";

    // --- CAMADA 7: ANÁLISE COMPORTAMENTAL E IDENTIDADE (K13-K16) ---
    trackerBases << "mixpanel.com" << "amplitude.com" << "segment.com" << "segment.io"
                 << "fullstory.com" << "quantserve.com" << "quantcast.com"
                 << "fingerprintjs.com" << "fpjs.io" << "api.fpjs.io" << "siftscience.com" 
                 << "cdn.siftscience.com" << "sift.com" << "permutive.com" 
                 << "cdn.permutive.com" << "onetag-sys.com" << "pippio.com" 
                 << "id5-sync.com" << "thetradedesk.com" << "mathtag.com" 
                 << "sync.mathtag.com" << "pixel.mathtag.com" << "tapad.com" 
                 << "prod.uidapi.com" << "bluekai.com" << "crwdcntrl.net";

    // --- CAMADA 8: ATRIBUIÇÃO MÓVEL, LIVERAMP E PUSH (K19) ---
    trackerBases << "rlcdn.com" << "api.rlcdn.com" << "idsync.rlcdn.com" 
                 << "appsflyer.com" << "app.appsflyer.com" << "adjust.com" 
                 << "app.adjust.com" << "branch.io" << "api2.branch.io" 
                 << "bnc.lt" << "kochava.com" << "control.kochava.com" 
                 << "singular.net" << "wzrkt.com" << "clevertap-prod.com" 
                 << "liveramp.com";

    // --- CAMADA 9: MALVERTISING E CRIPTOMINADORES (K21, X1, X2) ---
    trackerBases << "coinimp.com" << "crypto-loot.org" << "monerominer.rocks" 
                 << "popads.net" << "popcash.net" << "propellerclick.com" 
                 << "onclickads.net" << "popmyads.com" << "clickadu.com" 
                 << "trafficjunky.net" << "exoclick.com" << "juicyads.com"
                 << "2giga.link" << "greatis.com" << "statdynamic.com"
                 << "webminepool.com" << "minero.cc" << "mineralt.io" << "jsecoin.com";

    // --- CAMADA DE CAMINHOS (PATH BLACKLIST) ---
    pathBlacklist << "/ads/" << "/adv/" << "/metrics" << "/telemetry" << "/tracker"
                  << "/pixel" << "/collect" << "/log-event" << "/ad-delivery"
                  << "/adserver" << "/prebid" << "/amazon-adsystem" << "/google-ads"
                  << "/tr/" << "/p/" << "/v1/" << "/v2/" << "/mvt/" << "/sync"
                  << "/events/" << "/batch" << "/error-reporting" << "/delivery/"
                  << "/track/" << "/bid" << "/js-agent" << "/nr-ext" << "/gtm.js"
                  << "/ptracking" << "/user-sync" << "/identify" << "/fs.js" << "/fp.js"
                  << "/rtrg" << "/cookie-law" << "/consent" << "/vast" << "/vpaid";
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QString urlStr = info.requestUrl().toString().toLower();
    QString host = info.requestUrl().host().toLower();
    QString path = info.requestUrl().path().toLower();

    // --- FILTRO CIRÚRGICO: BLOQUEIO IMEDIATO DE ASSINATURAS CRÍTICAS ---
    if (host.contains("ads.huawei.com") || 
        host.contains("metrics.mzstatic.com") ||
        host.contains("device-metrics-us-2.amazon") ||
        host.contains("imasdk.googleapis.com") ||
        host.contains("dai.google.com") ||
        host.contains("connatix.com") ||
        host.contains("jwpsrv.com") ||
        host.contains("cookielaw.org") || 
        host.contains("cookiebot.com") ||
        host.contains("onetrust.com") ||
        host.contains("usercentrics.eu") ||
        host.contains("ngfts.lge.com") || 
        host.contains("vortex.data.microsoft") ||
        host.contains("mads-eu.amazon") ||
        host.contains("app-measurement.com") ||
        host.contains("snap.licdn.com") || 
        host.contains("ads.x.com") || 
        host.contains("pixel.quora.com") ||
        host.contains("trafficjunky") ||
        host.contains("bam.nr-data.net") || 
        host.contains("redirector.googlevideo.com") ||
        urlStr.contains("adformat") || 
        urlStr.contains("adunit") ||
        urlStr.contains("fingerprint") ||
        urlStr.contains("appsflyer") ||
        urlStr.contains("impactradius")) {
        
        info.block(true);
        globalAdsBlocked++;
        return;
    }

    // --- VERIFICAÇÃO GLOBAL: BASE, REFORÇO, PATH E ASSINATURA ---
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
        "fullstory", "quantserve", "mixpanel", "amplitude", "segment", "permutive",
        "trafficjunky", "sc-static", "instagram", "tumblr", "vk.com", "mail.ru",
        "popads", "popcash", "exoclick", "juicyads", "licdn", "quora", "huawei", 
        "cookie", "impact", "awin", "rakuten", "jwplayer", "brightcove", "connatix"
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