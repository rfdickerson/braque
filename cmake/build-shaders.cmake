# Find glslc shader compiler
find_program(GLSLC glslc HINTS Vulkan::glslc)

# Function to compile shaders
function(compile_shader TARGET SHADER)
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/assets/shaders/${SHADER_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSLC} -o ${SPIRV} -g --target-env=vulkan1.2 ${SHADER}
            DEPENDS ${SHADER}
            COMMENT "Compiling ${SHADER_NAME}"
    )
    target_sources(${TARGET} PRIVATE ${SPIRV})
endfunction()

# Function to compile all shaders in a directory
function(compile_shaders TARGET)
    file(GLOB_RECURSE SHADERS
            "${PROJECT_SOURCE_DIR}/assets/shaders/*.vert"
            "${PROJECT_SOURCE_DIR}/assets/shaders/*.frag"
    )
    foreach(SHADER ${SHADERS})
        compile_shader(${TARGET} ${SHADER})
    endforeach()

endfunction()