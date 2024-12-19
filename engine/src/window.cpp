//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/window.hpp"

#include <spdlog/spdlog.h>

#include "braque/renderer.hpp"

namespace braque {

Window::Window(const int width, const int height, const std::string &title) :
    window(nullptr), width(width), height(height) {

    glfwInit();

    // check for Vulkan support
    if (glfwVulkanSupported() == 0) {
        spdlog::error("Vulkan not supported");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create window
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        spdlog::error("Failed to create window");
        return;
    }

    spdlog::info("Window created");
}

Window::~Window() {

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }

    glfwTerminate();

    spdlog::info("Window destroyed");
}

auto Window::shouldClose() const -> bool { return glfwWindowShouldClose(window) != 0; }

void Window::pollEvents() { glfwPollEvents(); }

auto Window::createSurface(const Renderer &renderer) const -> vk::SurfaceKHR {
    VkSurfaceKHR surface = nullptr;

    auto result = glfwCreateWindowSurface(renderer.getInstance(), window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        spdlog::error("Failed to create window surface {}", std::to_string(result));
        throw std::runtime_error("Failed to create window surface");
    }

    return {surface};
}


} // namespace braque
