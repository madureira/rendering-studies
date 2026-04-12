cmake_minimum_required(VERSION 3.24)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Run cmake with a separate build directory, e.g.:\n"
        "  cmake -S . -B build/Debug")
endif()

project(rendering-studies)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# ============================================================
# DEPENDENCIES
# ============================================================

# --- Assimp: compiled from source submodule (both desktop and web) ---
set(ASSIMP_BUILD_TESTS                  OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS           OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL                      OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT                    ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER           ON  CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER           ON  CACHE BOOL "" FORCE)
# Always build assimp as a static library to avoid dynamic linker overhead
set(BUILD_SHARED_LIBS                   OFF CACHE BOOL "" FORCE)
if(EMSCRIPTEN)
    # Web: no system zlib available, build assimp's bundled one
    set(ASSIMP_BUILD_ZLIB ON  CACHE BOOL "" FORCE)
else()
    # Desktop: use system zlib to avoid bundled zlib's macOS SDK conflicts
    set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
endif()
add_subdirectory(vendor/assimp)

if(EMSCRIPTEN)
    # Silence Clang/Emscripten warnings inside assimp's bundled zlib
    target_compile_options(assimp PRIVATE -Wno-nontrivial-memcall)
    if(TARGET zlibstatic)
        target_compile_options(zlibstatic PRIVATE -Wno-deprecated-non-prototype)
    endif()
else()
    # Silence warnings in assimp's own code (not our problem to fix)
    target_compile_options(assimp PRIVATE -Wno-deprecated-copy)
    # Desktop: use system zlib to avoid bundled zlib's macOS SDK conflicts
    find_package(ZLIB REQUIRED)

    # --- GLFW: compiled from source submodule ---
    set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(vendor/glfw)

    # --- Freetype: compiled from source submodule ---
    set(FT_DISABLE_ZLIB          OFF CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2         ON  CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG           ON  CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ      ON  CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI        ON  CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL         ON  CACHE BOOL "" FORCE)
    add_subdirectory(vendor/freetype)
endif()

# ============================================================
# SOURCES
# ============================================================

file(GLOB_RECURSE CPP_SOURCES    src/*.cpp)
file(GLOB_RECURSE HEADER_FILES   src/*.h)

# ImGui compiled from source submodule (both builds)
set(IMGUI_SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    ${IMGUI_SOURCES}
    ${CPP_SOURCES} ${HEADER_FILES}
)

if(NOT EMSCRIPTEN)
    # glad: vendored generated sources (desktop only; web uses GLES3)
    target_sources(${PROJECT_NAME} PRIVATE vendor/glad/src/glad.c)
endif()

# ============================================================
# INCLUDE DIRECTORIES
# ============================================================
target_include_directories(${PROJECT_NAME} PRIVATE
    includes
    vendor
    vendor/imgui
    vendor/imgui/backends
    ${CMAKE_SOURCE_DIR}/vendor/glm
    ${CMAKE_SOURCE_DIR}/vendor/spdlog/include
)

if(NOT EMSCRIPTEN)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/vendor/glad/include
    )
endif()

# ============================================================
# ASSETS
# ============================================================
if(NOT EMSCRIPTEN)
    file(COPY assets     DESTINATION ${CMAKE_BINARY_DIR}/)
    file(COPY config.ini DESTINATION ${CMAKE_BINARY_DIR}/)
endif()

# ============================================================
# LINK LIBRARIES
# ============================================================

target_link_libraries(${PROJECT_NAME} assimp)

if(EMSCRIPTEN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_ES3)

    target_compile_options(${PROJECT_NAME} PRIVATE "-sUSE_FREETYPE=1")

    set(EM_LINK_FLAGS
        "-sUSE_WEBGL2=1"
        "-sUSE_GLFW=3"
        "-sUSE_FREETYPE=1"
        "-sFULL_ES3=1"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sINITIAL_MEMORY=134217728"
        "-sSTACK_SIZE=8388608"
        "--preload-file=${CMAKE_SOURCE_DIR}/assets@/assets"
        "--preload-file=${CMAKE_SOURCE_DIR}/config.ini@/config.ini"
        "--use-preload-plugins"
        "--shell-file=${CMAKE_SOURCE_DIR}/assets/index.html"
    )
    string(JOIN " " EM_LINK_FLAGS_STR ${EM_LINK_FLAGS})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX      ".html"
        LINK_FLAGS  "${EM_LINK_FLAGS_STR}"
    )
else()
    target_link_libraries(${PROJECT_NAME} freetype)
    target_link_libraries(${PROJECT_NAME} glfw)
endif()

# ============================================================
# PRECOMPILED HEADERS (desktop only — Emscripten PCH support is limited)
# ============================================================
if(NOT EMSCRIPTEN)
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/includes/RenderingStudies/pch.h>)
endif()
