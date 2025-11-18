# Buscar solo los headers (no hay .cpp)
file(GLOB_RECURSE INCLUDE_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Crear librería de interfaz solo para exponer headers
add_library(include_lib INTERFACE)

# Agregar los headers a la librería (para que aparezcan en Visual Studio)
target_sources(include_lib INTERFACE ${INCLUDE_HEADERS})

# Organizar por carpetas en Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INCLUDE_HEADERS})

# Indicar dónde están los headers
target_include_directories(include_lib INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)
```

## Explicación:

- **`INTERFACE`**: Significa que es solo para headers, no se compila nada
- **`target_sources(... INTERFACE)`**: Agrega los headers para que aparezcan en Visual Studio
- **`source_group(TREE ...)`**: Organiza los headers en carpetas en el Explorador de soluciones
- **`target_include_directories(... INTERFACE)`**: Hace que estos headers estén disponibles para otros proyectos que enlacen con `include_lib`

## Resultado en Visual Studio:
```
📦 include_lib (Visual Studio 2022)
  └─ 📂 Header Files
     └─ models.hpp
```

Si tienes más headers en subcarpetas dentro de `include/`, se mostrarán con su estructura:
```
📦 include_lib (Visual Studio 2022)
  └─ 📂 Header Files
     ├─ models.hpp
     └─ 📂 subcarpeta
        └─ otro.hpp