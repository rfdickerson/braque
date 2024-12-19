

add_library(braque STATIC
        src/engine.cpp
        src/renderer.cpp
        src/window.cpp
        src/swapchain.cpp
        src/rendering_stage.cpp
        src/debug_window.cpp
        src/memory_allocator.cpp
        src/image.cpp
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
)