//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace braque {

class Renderer;

constexpr int DEFAULT_WIDTH = 1280;
constexpr int DEFAULT_HEIGHT = 720;

class Window {
public:
    explicit Window(int width=DEFAULT_WIDTH, int height=DEFAULT_HEIGHT, const std::string& title="Braque");
    ~Window();

    // copy and move are deleted
    Window(const Window &) = delete;
    auto operator=(const Window &) -> Window & = delete;
    Window(Window &&) = delete;
    auto operator=(Window &&) -> Window & = delete;

    [[nodiscard]] auto ShouldClose() const -> bool;
    static void PollEvents();
    [[nodiscard]] auto CreateSurface(const Renderer &renderer) const -> vk::SurfaceKHR;
    [[nodiscard]] auto GetNativeWindow() const -> GLFWwindow * { return window; }
    [[nodiscard]] glm::vec2 GetMouseChange();
    [[nodiscard]] std::vector<int> GetPressedKeys() const;

    void HideCursor();
    void ShowCursor();

private:
    GLFWwindow *window;

    int width;
    int height;

    // store the last mouse position
    glm::vec2 last_mouse_position_;
    bool first_mouse_ = true;
};

} // namespace braque

#endif // WINDOW_HPP
