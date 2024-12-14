//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "window.hpp"

#include <spdlog/spdlog.h>

#include "renderer.hpp"

namespace braque {

Window::Window(): window(nullptr) {

    glfwInit();

    // check for Vulkan support
    if (!glfwVulkanSupported()) {
        spdlog::error("Vulkan not supported");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create window
    window = glfwCreateWindow(1280, 720, "Braque", nullptr, nullptr);
    if (!window) {
        spdlog::error("Failed to create window");
        return;
    }

    spdlog::info("Window created");

}

Window::~Window() {

    if (window) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();

    spdlog::info("Window destroyed");
}

bool Window::shouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
    glfwPollEvents();
}

vk::SurfaceKHR Window::createSurface(Renderer& renderer) {
    VkSurfaceKHR surface;

    auto result = glfwCreateWindowSurface(renderer.getInstance(), window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        spdlog::error("Failed to create window surface {}", std::to_string(result));
        throw std::runtime_error("Failed to create window surface");
    }

    return vk::SurfaceKHR(surface);
}


} // braque