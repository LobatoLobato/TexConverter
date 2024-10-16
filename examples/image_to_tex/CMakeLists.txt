add_executable(ImageToTexEx image_to_tex.cpp)
target_link_libraries(ImageToTexEx TexConverter)
configure_file(wurt.png ${CMAKE_BINARY_DIR}/examples/image_to_tex COPYONLY)

