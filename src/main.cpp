#include <QApplication>
#include "ui/browserwindow.h" // Incluímos o header da nossa janela principal

int main(int argc, char *argv[])
{
    // 1. Cria o objeto QApplication, que gerencia os recursos da aplicação e o loop de eventos.
    QApplication a(argc, argv);
    
    // **OPTIONAL: Otimização de Performance**
    // Se quisermos minimizar o uso de recursos, podemos aplicar atributos Qt de baixo nível aqui.
    // Exemplo para desabilitar hints visuais complexos que podem consumir CPU/RAM:
    a.setAttribute(Qt::AA_DontUseNativeMenuBar); 
    // a.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings); 

    // 2. Cria uma instância da nossa janela personalizada (BrowserWindow).
    BrowserWindow w;
    
    // 3. Exibe a janela na tela.
    w.show(); 

    // 4. Inicia o loop de eventos principal da aplicação Qt. O programa fica ativo aqui.
    return a.exec();
}