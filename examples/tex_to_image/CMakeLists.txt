add_executable(TexToImageEx tex_to_image.cpp)
target_link_libraries(TexToImageEx TexConverter)
configure_file(wurt.tex ${CMAKE_BINARY_DIR}/examples/tex_to_image COPYONLY)

