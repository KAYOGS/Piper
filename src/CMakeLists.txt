# Define a versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# --- Configuração MOC Explícita ---
# Garante que os headers com Q_OBJECT sejam processados (MOC).
set(CMAKE_AUTOMOC ON) 

# Garante que os arquivos .qrc sejam processados (RCC - Resource Compiler).
set(CMAKE_AUTORCC ON) 

# --- Definição dos Arquivos ---

set(PIPER_SOURCES
    main.cpp
    ui/browserwindow.cpp
)

set(PIPER_HEADERS
    ui/browserwindow.h
)

# --- Definição dos Arquivos de Recursos ---
# Declaração do nosso arquivo QRC que lista os ícones.
set(PIPER_RESOURCES
    ../res/resources.qrc
)

# --- Criação do Executável ---
# ESTA DEVE SER A PRIMEIRA REFERÊNCIA AO ALVO 'Piper' no arquivo.
add_executable(Piper 
    ${PIPER_SOURCES} 
    ${PIPER_HEADERS}
)

# CORREÇÃO: target_sources AGORA está depois de add_executable.
# Isso resolve o erro: "Cannot specify sources for target 'Piper' which is not built by this project."
target_sources(Piper PRIVATE
    ${PIPER_RESOURCES}
)

# --- Configuração de Inclusão ---
target_include_directories(Piper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Linkagem das Bibliotecas ---

target_link_libraries(Piper PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
)