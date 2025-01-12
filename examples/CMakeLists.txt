add_executable(demo src/demo.cc)
add_executable(packer src/packer.cc)

target_link_libraries(demo braque)
target_link_libraries(packer braque)

compile_shaders(demo)
compile_shaders(packer)