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
    // --- 1. BLOQUEIO DE PERFORMANCE (ADS, TRACKERS, FONTS) ---
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

    fontExtensions << ".woff" << ".woff2" << ".ttf" << ".otf" << ".eot";

    blacklistedPaths << "/wp-content/plugins/ad-rotator/" 
                     << "/vpaid/" << "/vast-xml/" << "/ad-delivery/";

    // --- 2. BLOQUEIO ÉTICO: APOSTAS E CASSINOS ---
    bettingKeywords << "bet" << "casino" << "cassino" << "slot" << "poker" 
                    << "blaze" << "brazino" << "777" << "pixbet" << "1xbet" 
                    << "betano" << "sportingbet" << "betfair" << "bodog" 
                    << "tigrinho" << "fortune-tiger" << "ganha-bet" << "lucky";

    // --- 3. BLOQUEIO ÉTICO: CONTEÚDO ADULTO (+18) ---
    adultKeywords << "porn" << "xvideo" << "redtube" << "pornhub" << "sexo" 
                  << "hentai" << "rule34" << "xxx" << "erotico" << "sexy" 
                  << "cam4" << "onlyfans" << "stripchat" << "xhamster" 
                  << "brazzers" << "beeg" << "xnxx" << "tnaflix" << "mineirinha"
                  << "garotas-de-programa" << "acompanhantes" << "putaria";

    ethicalHosts << "uol.com.br/sexo" << "terrasexo.com.br" << "privacy.com.br";
}

void UrlInterceptor::applyGlobalOptimizations(QWebEngineUrlRequestInfo &info)
{
    // 1. User Agent de ChromeOS: Equilíbrio entre Desktop e Otimização de hardware limitado
    QByteArray chromeOS_UA = "Mozilla/5.0 (X11; CrOS x86_64 14541.0.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";
    info.setHttpHeader("User-Agent", chromeOS_UA);

    // 2. Header Save-Data: Avisa ao site para enviar menos lixo e imagens comprimidas
    info.setHttpHeader("Save-Data", "on");
}

bool UrlInterceptor::isEthicalBlock(const QString &urlStr, const QString &host) const
{
    // --- EXCEÇÃO PARA SITES ESSENCIAIS (Evita falsos positivos como o do YouTube/Google) ---
    if (host.contains("youtube.com") || host.contains("google.com") || 
        host.contains("gstatic.com") || host.contains("ytimg.com")) {
        return false;
    }

    // Verifica Apostas
    for (const QString &key : bettingKeywords) {
        if (urlStr.contains(key) || host.contains(key)) return true;
    }

    // Verifica Conteúdo Adulto
    for (const QString &key : adultKeywords) {
        if (urlStr.contains(key) || host.contains(key)) return true;
    }

    // Verifica Hosts específicos
    for (const QString &eHost : ethicalHosts) {
        if (host.contains(eHost)) return true;
    }

    return false;
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QUrl url = info.requestUrl();
    QString host = url.host().toLower();
    QString urlStr = url.toString().toLower();
    QString path = url.path().toLower();

    // APLICA OTIMIZAÇÃO GLOBAL (UA ChromeOS + Save-Data)
    applyGlobalOptimizations(info);

    // 1. BLOQUEIO ÉTICO PRIORITÁRIO (Zero tolerância para Apostas e +18)
    if (isEthicalBlock(urlStr, host)) {
        info.block(true);
        qDebug() << "[PIPER ETHICS] Bloqueado (Site Proibido):" << host;
        return;
    }

    // 2. EXCEÇÕES DE SEGURANÇA (WHITELIST)
    if (host.contains("google.com/recaptcha") || host.contains("gstatic.com/recaptcha")) {
        return;
    }

    bool block = false;
    
    // 3. BLOQUEIO DE PERFORMANCE: HOSTS (Ads/Fonts)
    for (const QString &adHost : adHosts) {
        if (host == adHost || host.endsWith("." + adHost)) {
            block = true;
            break;
        }
    }

    // 4. BLOQUEIO DE PERFORMANCE: FONTES (.woff2, etc)
    if (!block) {
        for (const QString &ext : fontExtensions) {
            if (path.endsWith(ext)) {
                block = true;
                break;
            }
        }
    }

    // 5. BLOQUEIO DE PERFORMANCE: PADRÕES (Scripts de Ad)
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
        // Log comentado para não poluir o terminal, apenas o Ethics fica ativo
        qDebug() << "[PIPER OPTIMIZER] Bloqueado (Recurso Pesado):" << host;
    }
}