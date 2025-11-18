# --- Buscar todos los .cpp y headers dentro de constants ---
file(GLOB_RECURSE CONSTANTS_SOURCES #file es 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
#file → Comando de CMake para operar con archivos (copiar, buscar, leer, etc.).
#GLOB_RECURSE → Busca todos los archivos que coincidan con un patrón recursivamente en los subdirectorios.
#CONSTANTS_SOURCES → Variable de CMake donde se almacenan los archivos encontrados.
#"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" → Patrón de búsqueda:
#CMAKE_CURRENT_SOURCE_DIR → Directorio donde se encuentra el CMakeLists.txt actual.
#*.cpp → Todos los archivos con extensión .cpp.
#Resultado: CONSTANTS_SOURCES contendrá todos los .cpp dentro de constants y sus subcarpetas.

file(GLOB_RECURSE CONSTANTS_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)


# --- Crear la librería constants_lib ---
add_library(constants_lib STATIC
    ${CONSTANTS_SOURCES}
    ${CONSTANTS_HEADERS}
)
#add_library → Crea una librería en CMake.
#constants_lib → Nombre de la librería.
#STATIC → Tipo de librería:
#STATIC → Librería estática (.lib o .a), se enlaza en tiempo de compilación.
#SHARED → Librería dinámica (.dll o .so).
#${CONSTANTS_SOURCES} y ${CONSTANTS_HEADERS} → Archivos que se van a compilar o incluir en la librería.

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CONSTANTS_SOURCES} ${CONSTANTS_HEADERS})
#source_group → Agrupa archivos para que se vean organizados en IDEs como Visual Studio.
#TREE ${CMAKE_CURRENT_SOURCE_DIR} → Mantiene la estructura de carpetas original desde el directorio actual.
#FILES ${CONSTANTS_SOURCES} ${CONSTANTS_HEADERS} → Archivos que se agruparán.
#Resultado: En Visual Studio, los archivos .cpp y .h se mostrarán en carpetas de forma jerárquica.

target_include_directories(constants_lib PUBLIC
    ${CMAKE_SOURCE_DIR}
)
#target_include_directories → Indica a CMake dónde buscar los headers al compilar.
#constants_lib → Target al que se le aplica.
#PUBLIC → Nivel de visibilidad:
#PUBLIC → El directorio se usa tanto para compilar la librería como para quien use la librería.
#PRIVATE → Solo se usa para compilar la librería.
#INTERFACE → Solo se expone a quien usa la librería, pero no se compila dentro de ella.
#${CMAKE_SOURCE_DIR} → Directorio raíz del proyecto.