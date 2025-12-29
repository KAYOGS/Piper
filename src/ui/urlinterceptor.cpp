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

bool UrlInterceptor::isEthicalBlock(const QString &urlStr, const QString &host) const
{
    // --- PROTEÇÃO PARA O YOUTUBE E GOOGLE ---
    // Impede que o "tube" ou "sexy" (em outros contextos) bloqueie sites legítimos
    if (host.contains("youtube.com") || host.contains("googlevideo.com") || 
        host.contains("ytimg.com") || host.contains("google.com")) {
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

    // Verifica Hosts específicos da Ethical List
    for (const QString &eHost : ethicalHosts) {
        if (host.contains(eHost)) return true;
    }

    return false;
}

bool UrlInterceptor::isYouTubeRequest(const QUrl &url) const
{
    QString host = url.host().toLower();
    return host.contains("youtube.com") || host.contains("googlevideo.com");
}

void UrlInterceptor::applyLegacyUserAgent(QWebEngineUrlRequestInfo &info)
{
    // Força H.264 no YouTube para economizar CPU em hardware antigo
    QByteArray legacyUA = "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.1.2 Safari/605.1.15";
    info.setHttpHeader("User-Agent", legacyUA);
}

void UrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    QUrl url = info.requestUrl();
    QString host = url.host().toLower();
    QString urlStr = url.toString().toLower();
    QString path = url.path().toLower();

    // 1. BLOQUEIO ÉTICO PRIORITÁRIO
    if (isEthicalBlock(urlStr, host)) {
        info.block(true);
        qDebug() << "[PIPER ETHICS] Bloqueado (Site Proibido):" << host;
        return;
    }

    // 2. OTIMIZAÇÃO YOUTUBE (USER-AGENT SPOOFING)
    if (isYouTubeRequest(url)) {
        applyLegacyUserAgent(info);
        // Não bloqueamos nada dentro do domínio do vídeo para não quebrar o player
        if (host.contains("googlevideo.com")) return;
    }

    // 3. EXCEÇÕES TÉCNICAS (RECAPTCHA E ESSENCIAIS)
    if (host.contains("google.com/recaptcha") || host.contains("gstatic.com/recaptcha")) {
        return;
    }

    bool block = false;
    
    // 4. BLOQUEIO DE PERFORMANCE: HOSTS (Ads/Fonts)
    for (const QString &adHost : adHosts) {
        if (host == adHost || host.endsWith("." + adHost)) {
            block = true;
            break;
        }
    }

    // 5. BLOQUEIO DE PERFORMANCE: FONTES (.woff2, etc)
    if (!block) {
        for (const QString &ext : fontExtensions) {
            if (path.endsWith(ext)) {
                block = true;
                break;
            }
        }
    }

    // 6. BLOQUEIO DE PERFORMANCE: PADRÕES (Scripts de Ad)
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
        // Log para debug de performance se necessário
        qDebug() << "[PIPER OPTIMIZER] Bloqueado (Recurso Pesado):" << host;
    }
}