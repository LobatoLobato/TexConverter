cmake_minimum_required(VERSION 3.28)
project(TexConverter)

set(CMAKE_CXX_STANDARD 20)

add_library(TexConverter src/Converter.cpp)

target_include_directories(TexConverter PUBLIC src/include)

target_link_libraries(TexConverter PUBLIC KleiLib Image PRIVATE stb libsquish::Squish)

add_subdirectory(vendor)
add_subdirectory(examples)
