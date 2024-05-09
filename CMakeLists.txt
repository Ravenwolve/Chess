cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

SET(PROJECT_NAME Chess)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
    src/Main.cpp
    src/Bitboard.cpp
    )

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)

target_include_directories(${PROJECT_NAME} PRIVATE includes)
target_include_directories(${PROJECT_NAME} PRIVATE src)