cmake_minimum_required(VERSION 3.28)
project(stb)

set(CMAKE_CXX_STANDARD 20)

add_library(stb STATIC stb.cpp)

target_include_directories(stb PUBLIC include)