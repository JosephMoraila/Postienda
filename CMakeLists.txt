cmake_minimum_required(VERSION 3.22)
project(Postienda LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------
# Configuración para Windows
# -----------------------------------
if(WIN32)
    # Definiciones generales de wxWidgets
    add_definitions(
        -DUNICODE
        -D_UNICODE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING
        -D__WXMSW__
        -DwxUSE_GUI=1
    )

    # Definiciones específicas para Debug
    add_compile_definitions(
        $<$<CONFIG:Debug>:__WXDEBUG__>
        $<$<CONFIG:Debug>:WXDEBUG=1>
    )

    # Definir WXWIN si no está definido
    if(NOT DEFINED WXWIN)
        if(DEFINED ENV{WXWIN})
            set(WXWIN $ENV{WXWIN})
        else()
            message(FATAL_ERROR "WXWIN no definido. Debe apuntar a la carpeta de wxWidgets.")
        endif()
    endif()

    message(STATUS "WXWIN = ${WXWIN}")

    # Rutas a librerías y setup.h según configuración
    set(WX_LIB_DIR_DEBUG "${WXWIN}/lib/vc_x64_libdebug")
    set(WX_SETUP_DIR_DEBUG "${WX_LIB_DIR_DEBUG}/mswud")

    set(WX_LIB_DIR_RELEASE "${WXWIN}/lib/vc_x64_librelease")
    set(WX_SETUP_DIR_RELEASE "${WX_LIB_DIR_RELEASE}/mswu")

    # FORZAR las rutas correctas con defines
    add_compile_definitions(
        $<$<CONFIG:Debug>:wxUSE_LIB_SUFFIX=ud>
    )

    # Incluir headers generales
    include_directories(
        ${WXWIN}/include
    )

    # Incluir headers setup.h específicos por configuración ANTES del msvc
    include_directories("$<$<CONFIG:Debug>:${WX_SETUP_DIR_DEBUG}>")
    include_directories("$<$<CONFIG:Release>:${WX_SETUP_DIR_RELEASE}>")
    
    include_directories(${WXWIN}/include/msvc)

    # Librerías según configuración
    link_directories("$<$<CONFIG:Debug>:${WX_LIB_DIR_DEBUG}>")
    link_directories("$<$<CONFIG:Release>:${WX_LIB_DIR_RELEASE}>")
endif()

# -----------------------------------
# Configuración para Linux/Unix
# -----------------------------------
if(UNIX AND NOT APPLE)

    find_program(WX_CONFIG wx-config)
    if(NOT WX_CONFIG)
        message(FATAL_ERROR "No se encontró wx-config. Asegúrate de que wxWidgets esté instalado y que su bin esté en PATH.")
    endif()

    message(STATUS "Usando wx-config en: ${WX_CONFIG}")

    # COMPONENTES requeridos por tu aplicación
    set(WX_COMPONENTS "std,richtext,stc,html,xrc")

    execute_process(
        COMMAND ${WX_CONFIG} --cxxflags
        OUTPUT_VARIABLE WX_CXXFLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND ${WX_CONFIG} --libs ${WX_COMPONENTS}
        OUTPUT_VARIABLE WX_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    message(STATUS "Flags de compilación wxWidgets: ${WX_CXXFLAGS}")
    message(STATUS "Librerías wxWidgets: ${WX_LIBS}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WX_CXXFLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WX_LIBS}")

    # GLib + GObject + GIO
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLIB REQUIRED glib-2.0 gobject-2.0 gio-2.0)

    include_directories(${GLIB_INCLUDE_DIRS})
    link_directories(${GLIB_LIBRARY_DIRS})
    add_definitions(${GLIB_CFLAGS_OTHER})

    # CUPS
    pkg_check_modules(CUPS REQUIRED cups)
    include_directories(${CUPS_INCLUDE_DIRS})

    # LIBUSB
    pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
    include_directories(${LIBUSB_INCLUDE_DIRS})
    link_directories(${LIBUSB_LIBRARY_DIRS})

endif()

# -----------------------------------
# Subdirectorios del proyecto
# -----------------------------------
add_subdirectory(third_party)
add_subdirectory(utils)
add_subdirectory(print)
add_subdirectory(constants)
add_subdirectory(gui)
add_subdirectory(src)

# -----------------------------------
# Ejecutable principal
# -----------------------------------
add_executable(Postienda src/App.cpp)

# -----------------------------------
# Includes
# -----------------------------------
if(UNIX AND NOT APPLE)
    target_include_directories(Postienda PRIVATE
        ${GLIB_INCLUDE_DIRS}
        ${CUPS_INCLUDE_DIRS}
        ${LIBUSB_INCLUDE_DIRS}
    )
endif()

# -----------------------------------
# Librerías internas y externas
# -----------------------------------
target_link_libraries(Postienda PRIVATE
    gui_lib
    print_lib
    utils_lib
    constants_lib
    sqlite3
    nlohmann_json
)

# Librerías Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(Postienda PRIVATE
        ${WX_LIBS}
        ${GLIB_LIBRARIES}
        ${CUPS_LIBRARIES}
        ${LIBUSB_LIBRARIES}

    )
endif()

# Librerías Windows
# Librerías Windows
# Librerías Windows
# Librerías Windows
if(WIN32)
    # Resolver conflictos de runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    
    # Librerías DEBUG (con sufijo 'd')
    set(WX_LIBS_DEBUG
        wxmsw33ud_richtext
        wxmsw33ud_html
        wxmsw33ud_core
        wxbase33ud
        wxbase33ud_xml
        wxpngd
        wxtiffd
        wxjpegd
        wxzlibd
        wxregexud
        wxexpatd
        comctl32
        rpcrt4
        winmm
    )
    
    # Librerías RELEASE (sin sufijo 'd')
    set(WX_LIBS_RELEASE
        wxmsw33u_richtext
        wxmsw33u_html
        wxmsw33u_core
        wxbase33u
        wxbase33u_xml
        wxpng
        wxtiff
        wxjpeg
        wxzlib
        wxregexu
        wxexpat
        comctl32
        rpcrt4
        winmm
    )

    set(WEBP_LIBS
        ${CMAKE_SOURCE_DIR}/third_party/webp/lib/libwebp.lib
        ${CMAKE_SOURCE_DIR}/third_party/webp/lib/libwebpdemux.lib
        ${CMAKE_SOURCE_DIR}/third_party/webp/lib/libwebpmux.lib
    )

    # Enlazar las librerías correctas según la configuración
    target_link_libraries(Postienda PRIVATE 
        $<$<CONFIG:Debug>:${WX_LIBS_DEBUG}>
        $<$<CONFIG:Release>:${WX_LIBS_RELEASE}>
        ${WEBP_LIBS}
    )
endif()

# -----------------------------------
# Definiciones por configuración
# -----------------------------------
target_compile_definitions(Postienda PRIVATE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# -----------------------------------
# Propiedades del ejecutable
# -----------------------------------
set_target_properties(Postienda PROPERTIES
    WIN32_EXECUTABLE TRUE
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Postienda)
