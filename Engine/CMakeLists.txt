# Engine/CMakeLists.txt
cmake_minimum_required(VERSION 3.24)

project(Engine)


# Add vendor libraries here
add_subdirectory(vendor)

# Gather all source files in src/
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
        src/*.cpp
        src/*.c
)

find_package(Vulkan REQUIRED)

add_library(engine STATIC ${ENGINE_SRC})
target_precompile_headers(engine PRIVATE src/spa_pch.h)
target_compile_definitions(engine PRIVATE SPA_EXPORTS)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL/include


)

target_link_libraries(engine PRIVATE Vulkan::Vulkan)
target_link_libraries(engine PRIVATE SDL3::SDL3-static)
target_link_libraries(engine PRIVATE spdlog::spdlog_header_only)
