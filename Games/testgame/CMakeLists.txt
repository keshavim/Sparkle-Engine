# Games/testgame/CMakeLists.txt
project(testgame)

file(GLOB_RECURSE TESTGAME_SRC CONFIGURE_DEPENDS src/*.cpp src/*.h)

add_executable(testgame ${TESTGAME_SRC})

target_include_directories(testgame PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/Engine/src
        ${CMAKE_SOURCE_DIR}/Engine/vendor
)

target_link_libraries(testgame PRIVATE engine)