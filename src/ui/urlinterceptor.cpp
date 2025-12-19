#include "urlinterceptor.h"
#include <QWebEngineUrlRequestInfo>
#include <QDebug>

// --- INICIALIZAÇÃO DOS CONTADORES ---
// Esta linha é crucial para que o compilador reserve espaço para o contador global
int UrlInterceptor::globalAdsBlocked = 0;

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
    if (url.host().contains("gstatic.com") || url.host().contains("accounts.google.com")) {
        return; 
    }
    
    // Verificação ultra-rápida por padrões
    if (isAd(url)) {
        info.block(true); // Cancela o pedido antes de usar internet ou RAM
        
        // Incrementa o contador global para o PiperHub
        globalAdsBlocked++; 
        
        qDebug() << "Piper Escudo [BLOQUEADO]:" << url.host() 
                 << "| Total acumulado:" << globalAdsBlocked;
    }
}

bool UrlInterceptor::isAd(const QUrl &url)
{
    // Converte para minúsculo uma única vez para poupar processamento
    QString urlString = url.toString().toLower();

    // Otimização: Se a URL contém termos da lista, bloqueia na hora
    for (const QString &pattern : adDomains) {
        if (urlString.contains(pattern)) {
            return true;
        }
    }

    return false;
}