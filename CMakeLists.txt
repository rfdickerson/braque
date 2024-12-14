cmake_minimum_required(VERSION 3.30)
project(braque)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Add vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "VCPKG toolchain file")

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_definitions(-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)

add_executable(braque src/main.cpp
        src/engine.cpp
        src/engine.hpp
        src/renderer.cpp
        src/renderer.hpp
        src/window.cpp
        src/window.hpp
        src/swapchain.cpp
        src/swapchain.hpp)

target_link_libraries(braque PRIVATE glfw glm::glm Vulkan::Vulkan Vulkan::Headers spdlog::spdlog)