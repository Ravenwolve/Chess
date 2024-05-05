cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

project(Chess)

add_executable(Chess
    src/Main.cpp
    src/Bitboard.cpp
    )

target_compile_options(Chess PRIVATE -std=c++20)