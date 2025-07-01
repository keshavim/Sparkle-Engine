cmake_minimum_required(VERSION 3.24)

# Force SDL to build statically
set(SDL_SHARED OFF CACHE BOOL "Build SDL as shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL as static library" FORCE)
set(SDL_VULKAN ON CACHE BOOL "Enable Vulkan support in SDL3" FORCE)

add_subdirectory(SDL)
add_subdirectory(spdlog)