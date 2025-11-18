# --- Buscar todos los .cpp y headers dentro de gui ---
file(GLOB_RECURSE GUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE GUI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# --- Crear la librería gui_lib ---
add_library(gui_lib STATIC
    ${GUI_SOURCES}
    ${GUI_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GUI_SOURCES} ${GUI_HEADERS})

target_include_directories(gui_lib PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(gui_lib PUBLIC
    utils_lib
    print_lib
    constants_lib
    nlohmann_json
    sqlite3
)