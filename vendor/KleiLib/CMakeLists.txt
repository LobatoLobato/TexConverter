cmake_minimum_required(VERSION 3.28)
project(KleiLib)

set(CMAKE_CXX_STANDARD 20)

add_library(KleiLib STATIC TexFile.cpp Mipmap.cpp)

target_include_directories(KleiLib PUBLIC include)
target_link_libraries(KleiLib PUBLIC BinaryTools libsquish::Squish Image)