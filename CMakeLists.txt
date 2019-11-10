cmake_minimum_required(VERSION 3.15)
project(tetris_evolution)

set(CMAKE_CXX_STANDARD 17)

add_executable(tetris_evolution main.cpp ai.cpp ai.h)