# Games/testgame/CMakeLists.txt
cmake_minimum_required(VERSION 3.24)

project(testgame)

file(GLOB_RECURSE TESTGAME_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(testgame ${TESTGAME_SRC})

target_include_directories(testgame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/Engine/src
)

target_link_libraries(testgame PRIVATE engine)