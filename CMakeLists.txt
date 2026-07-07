cmake_minimum_required(VERSION 3.20)

project(Chess)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(Chess
    "main.cpp"
    "game.cpp"
    "minimax.cpp"
)

