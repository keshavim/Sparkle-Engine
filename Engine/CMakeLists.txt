# Engine/CMakeLists.txt
project(Engine)

# Add vendor libraries here
add_subdirectory(vendor)

# Gather all source files in src/
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        src/*.cpp
        src/*.c
        src/*.h
        src/*.hpp
)

add_library(engine STATIC ${ENGINE_SRC})
target_precompile_headers(engine PRIVATE src/spa_pch.h)
target_compile_definitions(engine PRIVATE SPA_EXPORTS)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

target_link_libraries(engine PUBLIC SDL3::SDL3)

