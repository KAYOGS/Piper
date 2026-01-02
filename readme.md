# Piper Browser - Beta v0.1.0

O **Piper Browser** é um navegador leve e eficiente desenvolvido em **C++/Qt6**. Foi projetado especificamente para máquinas com hardware limitado, utilizando técnicas de otimização de recursos e uma interface simplificada para garantir uma navegação fluida.

## 🚀 Diferenciais Técnicos

* **Escada de Hibernação:** Sistema inteligente de gestão de memória que coloca abas inativas em hibernação após 2, 7 e 10 minutos, libertando RAM e CPU.
* **Low-End Optimization:** Mascaramento de User-Agent (ChromeOS) para forçar sites a entregarem versões mobile ou leves, otimizando o carregamento.
* **Motor Chromium (QtWebEngine):** Baseado no motor de renderização mais estável e compatível da atualidade.
* **Identidade Visual:** UI personalizada com botões de ação rápida e um botão de autor exclusivo.

## 🛠️ Instalação e Execução (Linux)

Para compilar e correr o Piper no teu sistema, abre o terminal dentro da pasta onde clonaste o projeto (`~/Piper`) e executa o comando unificado abaixo:

```bash
sudo apt update && sudo apt install -y build-essential qt6-base-dev qt6-webengine-dev libqt6webenginewidgets6 cmake && mkdir -p build && cd build && cmake .. && make -j$(nproc) && ./bin/Piper