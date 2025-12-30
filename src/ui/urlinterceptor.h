#ifndef URLINTERCEPTOR_H
#define URLINTERCEPTOR_H

#include <QWebEngineUrlRequestInterceptor>
#include <QStringList>
#include <QUrl>
#include <QSet>

/**
 * @class UrlInterceptor
 * @brief Gerencia o bloqueio ético (Apostas/Adulto), performance (Ads/Fontes)
 * e aplica otimizações globais de cabeçalho (ChromeOS + Save-Data).
 */
class UrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    explicit UrlInterceptor(QObject *parent = nullptr);
    
    /**
     * @brief Intercepta todas as requisições do motor WebEngine.
     * Implementa lógica de bloqueio silencioso e injeção de headers de otimização.
     */
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    // Contador global para estatísticas de otimização
    static int globalAdsBlocked;

private:
    // --- LISTAS DE BLOQUEIO DE PERFORMANCE ---
    QSet<QString> adHosts;       // Domínios de anúncios, rastreio e fontes
    QStringList adPatterns;      // Padrões de URL (ex: /ads.js)
    QStringList blacklistedPaths; // Caminhos proibidos
    QStringList fontExtensions;   // Extensões de fontes externas (.woff2, etc)

    // --- LISTAS DE BLOQUEIO ÉTICO ---
    QStringList bettingKeywords;  // Termos de apostas/cassinos
    QStringList adultKeywords;    // Termos de conteúdo adulto (+18)
    QSet<QString> ethicalHosts;   // Domínios específicos de apostas e adulto

    /**
     * @brief Inicializa as listas de bloqueio e padrões de otimização.
     */
    void initializeLists();

    /**
     * @brief Aplica o perfil global de ChromeOS e o header Save-Data.
     * Esta função otimiza a identidade do navegador para todos os sites.
     */
    void applyGlobalOptimizations(QWebEngineUrlRequestInfo &info);

    /**
     * @brief Verifica se a URL contém termos de apostas ou conteúdo adulto.
     */
    bool isEthicalBlock(const QString &urlStr, const QString &host) const;
};

#endif // URLINTERCEPTOR_H