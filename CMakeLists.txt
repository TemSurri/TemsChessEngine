cmake_minimum_required(VERSION 3.20)

project(Chess LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw/glfw-3.4)

add_library(glad
    external/glad/src/glad.c
 "gui/gui.h" "gui/shader.cpp" "gui/shader.h" "gui/ebo.h" "gui/ebo.cpp" "gui/vbo.cpp" "gui/vbo.h" "gui/vao.h" "gui/vao.cpp")

target_include_directories(glad PUBLIC
    external/glad/include
)

add_executable(Chess
    main.cpp
    game.cpp
    minimax.cpp
    "gui/gui.cpp"
 "gui/gui.h"
     "gui/shader.cpp" "gui/shader.h" "gui/ebo.h" "gui/ebo.cpp" "gui/vbo.cpp" "gui/vbo.h" "gui/vao.h" "gui/vao.cpp")

target_link_libraries(Chess PRIVATE
    glad
    glfw
    opengl32
)


add_custom_command(
    TARGET Chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:Chess>/resources"
)