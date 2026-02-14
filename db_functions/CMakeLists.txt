# --- Buscar todos los .cpp y headers dentro de db functions ---
file(GLOB_RECURSE DB_FUNCTIONS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE DB_FUNCTIONS_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# --- Crear la librería db_functions ---
add_library(db_functions STATIC
    ${DB_FUNCTIONS_SOURCES}   
    ${DB_FUNCTIONS_HEADERS}   
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DB_FUNCTIONS_SOURCES} ${DB_FUNCTIONS_HEADERS})

target_include_directories(db_functions PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(db_functions PUBLIC
    utils_lib
    print_lib
    constants_lib
    nlohmann_json
    sqlite3
)