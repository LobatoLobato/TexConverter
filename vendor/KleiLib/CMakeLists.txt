cmake_minimum_required(VERSION 3.28)
project(KleiLib)

set(CMAKE_CXX_STANDARD 20)

add_library(KleiLib STATIC TexFile/TexFile.cpp TexFile/TexFile.h)


set(TEMP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/KleiLib/include)
file(MAKE_DIRECTORY ${TEMP_INCLUDE_DIR})
file(COPY TexFile/TexFile.h DESTINATION ${TEMP_INCLUDE_DIR}/KleiLib/)
target_include_directories(KleiLib PUBLIC $<BUILD_INTERFACE:${TEMP_INCLUDE_DIR}>)

target_link_libraries(KleiLib PUBLIC BinaryTools)
