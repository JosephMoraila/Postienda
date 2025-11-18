cmake_minimum_required(VERSION 3.21)

# ==============================================
# 1) SQLITE3 (sí genera .lib)
# ==============================================
set(SQLITE3_SOURCES
    SQLite/sqlite3/sqlite3.c
    SQLite/sqlite3/shell.c
)

set(SQLITE3_HEADERS
    SQLite/sqlite3/sqlite3.h
    SQLite/sqlite3/sqlite3ext.h
    SQLite/sqlite3/sqlite_modern_cpp.h
)

add_library(sqlite3 STATIC
    ${SQLITE3_SOURCES}
    ${SQLITE3_HEADERS}
)

target_include_directories(sqlite3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SQLite/sqlite3
)

# Ocultar proyecto sqlite3 y ponerlo en carpeta third_party
set_target_properties(sqlite3 PROPERTIES
    FOLDER "third_party"
)

# ==============================================
# 2) WEBP (imported libs)
# ==============================================
set(WEBP_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/webp/lib")
set(WEBP_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/webp/include")

add_library(webp STATIC IMPORTED)
set_target_properties(webp PROPERTIES
    IMPORTED_LOCATION "${WEBP_LIB_DIR}/libwebp.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${WEBP_INCLUDE_DIR}"
    FOLDER "third_party"
)

add_library(webpmux STATIC IMPORTED)
set_target_properties(webpmux PROPERTIES
    IMPORTED_LOCATION "${WEBP_LIB_DIR}/libwebpmux.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${WEBP_INCLUDE_DIR}"
    FOLDER "third_party"
)

add_library(webpdemux STATIC IMPORTED)
set_target_properties(webpdemux PROPERTIES
    IMPORTED_LOCATION "${WEBP_LIB_DIR}/libwebpdemux.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${WEBP_INCLUDE_DIR}"
    FOLDER "third_party"
)

file(GLOB_RECURSE WEBP_HEADERS
    "${WEBP_INCLUDE_DIR}/*.h"
)

# ==============================================
# 3) JSON (solo header)
# ==============================================
add_library(nlohmann_json INTERFACE)
target_sources(nlohmann_json INTERFACE json.hpp)
target_include_directories(nlohmann_json INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET nlohmann_json PROPERTY FOLDER "third_party")

# ==============================================
# 4) Crear un solo grupo visual third_party con todos los archivos
# ==============================================

file(GLOB_RECURSE THIRD_PARTY_FILES
    ${SQLITE3_SOURCES}
    ${SQLITE3_HEADERS}
    ${WEBP_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
)

# Un target vacío que VS mostrará como un solo proyecto
add_custom_target(third_party_files SOURCES ${THIRD_PARTY_FILES})

# Agruparlos bajo una sola carpeta "third_party"
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "third_party" FILES ${THIRD_PARTY_FILES})

# Hacer que VS ponga este target en la carpeta third_party
set_property(TARGET third_party_files PROPERTY FOLDER "third_party")
