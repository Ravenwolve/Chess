cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

SET(PROJECT_NAME Chess)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
    src/Core/Bitboard.cpp
    src/Core/Position.cpp
    src/Core/Cache.cpp

    src/CLI/Image.cpp
    src/CLI/Console.cpp
    src/CLI/Main.cpp
    
    src/Logic/Logic.cpp
    )

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++20)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)