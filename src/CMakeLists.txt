# Define a versão mínima do CMake
cmake_minimum_required(VERSION 3.16)

# --- Configuração MOC Explícita ---
# Garante que os headers com Q_OBJECT sejam processados (MOC).
set(CMAKE_AUTOMOC ON) 

# Garante que os arquivos .qrc sejam processados (RCC - Resource Compiler).
set(CMAKE_AUTORCC ON) 

# --- Busca de Pacotes (Qt 6) ---
# Adicionamos WebEngineCore explicitamente para suportar as funções de download
find_package(Qt6 REQUIRED COMPONENTS 
    Widgets 
    WebEngineWidgets 
    WebEngineCore
)

# --- Definição dos Arquivos ---
set(PIPER_SOURCES
    main.cpp
    ui/browserwindow.cpp
    ui/urlinterceptor.cpp
)

set(PIPER_HEADERS
    ui/browserwindow.h
    ui/urlinterceptor.h
)

# --- Definição dos Arquivos de Recursos ---
set(PIPER_RESOURCES
    ../res/resources.qrc
)

# --- Criação do Executável ---
add_executable(Piper 
    ${PIPER_SOURCES} 
    ${PIPER_HEADERS}
)

# Adiciona os recursos ao executável
target_sources(Piper PRIVATE
    ${PIPER_RESOURCES}
)

# --- Configuração de Inclusão ---
target_include_directories(Piper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Linkagem das Bibliotecas ---
# Incluímos Qt6::WebEngineCore para resolver os erros de "diretório inexistente"
target_link_libraries(Piper PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebEngineCore
)