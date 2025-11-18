file(GLOB_RECURSE PRINT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE PRINT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(print_lib STATIC
    ${PRINT_SOURCES}
    ${PRINT_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PRINT_SOURCES} ${PRINT_HEADERS})

# Agregar rutas de include correctas
target_include_directories(print_lib PUBLIC
    ${CMAKE_SOURCE_DIR}          # raíz del proyecto
    ${CMAKE_SOURCE_DIR}/constants
    ${CMAKE_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}
)

