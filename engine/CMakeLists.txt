
set (braque-headers
        include/braque/engine.h
        include/braque/renderer.h
        include/braque/window.h
        include/braque/swapchain.h
        include/braque/rendering_stage.h
        include/braque/debug_window.h
        include/braque/memory_allocator.h
        include/braque/image.h
        include/braque/shader.h
        include/braque/pipeline.h
        include/braque/frame_stats.h
        include/braque/uniforms.h
        include/braque/camera.h
        include/braque/input/input_controller.h
        include/braque/input/fps_controller.h
        include/braque/input/event_controller.h
        include/braque/input/app_controller.h
        include/braque/scene.h
        include/braque/buffer.h
        include/braque/texture.h
        include/braque/engine_context.h
        include/braque/asset_loader.h
)

add_library(braque STATIC
        ${braque-headers}
        src/engine.cpp
        src/renderer.cpp
        src/window.cpp
        src/swapchain.cpp
        src/rendering_stage.cpp
        src/debug_window.cpp
        src/memory_allocator.cpp
        src/image.cpp
        src/shader.cpp
        src/pipeline.cpp
        src/frame_stats.cpp
        src/uniforms.cc
        src/camera.cc
        src/input/input_controller.cc
        src/input/fps_controller.cc
        src/input/app_controller.cpp
        src/scene.cc
        src/buffer.cc
        src/texture.cc
        src/asset_loader.cc
)

target_include_directories(braque PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${VULKAN_SDK}/include
)

target_link_libraries(braque
        glm::glm
        Vulkan::Vulkan
        Vulkan::Headers
        spdlog::spdlog
        imgui::imgui
        GPUOpen::VulkanMemoryAllocator
        gli
        Snappy::snappy
)

target_precompile_headers(braque PRIVATE
        <glm/glm.hpp>
        <gli/gli.hpp>
        <vulkan/vulkan.hpp>
        <vk_mem_alloc.h>
        <spdlog/spdlog.h>
)