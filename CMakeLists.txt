cmake_minimum_required(VERSION 3.15)
project(tetris_evolution)

set(CMAKE_CXX_STANDARD 17)
set( CMAKE_C_FLAGS_RELEASE        "-O3" )

add_executable(tetris_evolution main.cpp ai.cpp ai.h functions.cpp functions.h)