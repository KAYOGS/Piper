# --- Configuração MOC Explícita ---
# Usamos a abordagem mais robusta para forçar o MOC.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# --- Definição dos Arquivos ---

set(PIPER_SOURCES
    main.cpp
    ui/browserwindow.cpp
)

set(PIPER_HEADERS
    ui/browserwindow.h
)

# --- Criação do Executável ---
# Voltamos para add_executable normal, mas com CMAKE_AUTOMOC ON
add_executable(Piper ${PIPER_SOURCES} ${PIPER_HEADERS})

# --- Configuração de Inclusão ---
target_include_directories(Piper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Linkagem das Bibliotecas ---

target_link_libraries(Piper PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
)