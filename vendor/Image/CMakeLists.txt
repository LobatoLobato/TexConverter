cmake_minimum_required(VERSION 3.28)
project(Image)

set(CMAKE_CXX_STANDARD 20)

add_library(Image STATIC Image.cpp)

target_include_directories(Image PUBLIC include)
target_link_libraries(Image PUBLIC stb)
