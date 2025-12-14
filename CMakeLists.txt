# Define a versão mínima do CMake necessária. Usamos 3.16 que suporta as funcionalidades modernas do Qt6.
cmake_minimum_required(VERSION 3.16)

# Define o nome do projeto (Piper), a versão e a linguagem principal (C++).
project(Piper VERSION 1.0.0 LANGUAGES CXX)

# Define onde os executáveis (o binário 'Piper') serão colocados. 
# Eles vão para uma pasta 'bin' dentro do diretório 'build'.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Configurações do Qt ---
# Informa ao CMake para procurar a instalação do Qt6 e seus componentes necessários.
# - Widgets: Necessário para a interface gráfica básica (QMainWindow, QWidget).
# - WebKitWidgets: O motor de renderização leve que escolhemos.
#find_package(Qt6 REQUIRED COMPONENTS Widgets WebKitWidgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# Adiciona o subdiretório 'src' ao projeto principal. 
# Isso fará com que o CMake processe o 'src/CMakeLists.txt' na próxima etapa.
add_subdirectory(src)