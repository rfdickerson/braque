//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vulkan/vulkan.hpp>

namespace braque {

class Renderer;

class Window {
public:
    Window();
    ~Window();

    // copy and move are deleted
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    bool shouldClose();
    void pollEvents();
    vk::SurfaceKHR createSurface(Renderer& renderer);
    GLFWwindow* getNativeWindow() const { return window; }

private:
    GLFWwindow *window;
};

} // braque

#endif //WINDOW_HPP
