cmake_minimum_required(VERSION 3.20)
project(Sparkle)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add engine first
add_subdirectory(Engine)

# Add games
add_subdirectory(Games/testgame)
