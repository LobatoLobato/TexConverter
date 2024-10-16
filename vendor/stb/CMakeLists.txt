cmake_minimum_required(VERSION 3.28)
project(stb)

set(CMAKE_CXX_STANDARD 20)

add_library(stb STATIC stb.cpp stb_image.h stb_image_write.h)

target_include_directories(stb PUBLIC $<BUILD_INTERFACE:${TEMP_INCLUDE_DIR}>)

file(COPY stb_image.h stb_image_write.h DESTINATION ${TEMP_INCLUDE_DIR}/stb/)

set(TEMP_INCLUDE_DIR ${CMAKE_BINARY_DIR}/stb/include)
file(MAKE_DIRECTORY ${TEMP_INCLUDE_DIR})
file(COPY stb_image.h stb_image_write.h DESTINATION ${TEMP_INCLUDE_DIR}/stb/)
target_include_directories(KleiLib PUBLIC $<BUILD_INTERFACE:${TEMP_INCLUDE_DIR}>)