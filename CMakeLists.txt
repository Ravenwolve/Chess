cmake_minimum_required(VERSION 3.29.2 FATAL_ERROR)

project(Chess)

add_executable(Chess src/Main.cpp src/Bitboard.cpp src/PieceBitboardContainer.cpp)

target_compile_options(Chess PRIVATE -std=c++20)