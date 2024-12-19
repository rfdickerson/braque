cmake_minimum_required(VERSION 3.24)
project(braque VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "VCPKG toolchain file")

# add memory errors detection
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# generate debug information
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

add_definitions(-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

# make sure warnings are enabled
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

if(CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_executable(braque src/main.cpp
        src/engine.cpp
        src/engine.hpp
        src/renderer.cpp
        src/renderer.hpp
        src/window.cpp
        src/window.hpp
        src/swapchain.cpp
        src/swapchain.hpp
        src/rendering_stage.cpp
        src/rendering_stage.hpp
        src/debug_window.cpp
        src/debug_window.hpp
        src/memory_allocator.cpp
        src/memory_allocator.hpp
        src/image.cpp
        src/image.hpp)

target_link_libraries(braque PRIVATE
        glm::glm Vulkan::Vulkan Vulkan::Headers spdlog::spdlog imgui::imgui GPUOpen::VulkanMemoryAllocator)