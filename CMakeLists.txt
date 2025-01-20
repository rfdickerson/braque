cmake_minimum_required(VERSION 3.24)
project(braque VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "VCPKG toolchain file")

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanHeaders REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(gli CONFIG REQUIRED)
find_package(Snappy CONFIG REQUIRED)

add_definitions(-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
add_definitions(-DGLFW_INCLUDE_VULKAN)
add_definitions(-DVMA_STATIC_VULKAN_FUNCTIONS=0)
add_definitions(-DVMA_DYNAMIC_VULKAN_FUNCTIONS=1)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)

# make sure warnings are enabled
if (MSVC)
    add_compile_options(/W4)
else ()
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

include(cmake/build-shaders.cmake)

add_subdirectory(engine)
add_subdirectory(examples)
add_subdirectory(tests)
