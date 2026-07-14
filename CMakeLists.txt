cmake_minimum_required(VERSION 3.20)

project(Chess LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable unnecessary GLFW targets
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# GLFW
add_subdirectory(external/glfw/glfw-3.4)

# GLAD should contain only the GLAD implementation
add_library(glad STATIC
    external/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# Main executable
add_executable(Chess
    main.cpp
    game.cpp
    minimax.cpp

    gui/gui.cpp
    gui/shader.cpp

    external/stb/stb_image.cpp
)

target_include_directories(Chess PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/stb
)

# Platform-specific OpenGL libraries
if(WIN32)
    target_link_libraries(Chess PRIVATE
        glad
        glfw
        opengl32
    )
elseif(APPLE)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)

    target_link_libraries(Chess PRIVATE
        glad
        glfw
        ${OPENGL_FRAMEWORK}
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)

    target_link_libraries(Chess PRIVATE
        glad
        glfw
        OpenGL::GL
    )
endif()

# Copy resources beside the executable
add_custom_command(
    TARGET Chess POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:Chess>/resources"
)