//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/window.h"

#include <spdlog/spdlog.h>

#include "braque/renderer.h"

namespace braque {

Window::Window(const int width, const int height, const std::string& title)
    : window(nullptr), width(width), height(height), last_mouse_position_() {

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

auto Window::ShouldClose() const -> bool {
  return glfwWindowShouldClose(window) != 0;
}

void Window::PollEvents() {
  glfwPollEvents();
}

auto Window::CreateSurface(const Renderer& renderer) const -> vk::SurfaceKHR {
  VkSurfaceKHR surface = nullptr;

  auto result = glfwCreateWindowSurface(renderer.getInstance(), window, nullptr,
                                        &surface);
  if (result != VK_SUCCESS) {
    spdlog::error("Failed to create window surface {}", std::to_string(result));
    throw std::runtime_error("Failed to create window surface");
  }

  return {surface};
}

auto Window::GetMouseChange() -> glm::vec2 {
  double xpos{};
  double ypos{};
  glfwGetCursorPos(window, &xpos, &ypos);

  if (first_mouse_) {
    last_mouse_position_ = glm::vec2(xpos, ypos);
    first_mouse_ = false;
  }

  const glm::vec2 mouse_change = glm::vec2(xpos, ypos) - last_mouse_position_;
  last_mouse_position_ = glm::vec2(xpos, ypos);

  return mouse_change;
}

auto Window::GetPressedKeys() const -> std::vector<int> {
  std::vector<int> keys;
  for (int i = 0; i < GLFW_KEY_LAST; i++) {
    if (glfwGetKey(window, i) == GLFW_PRESS) {
      keys.push_back(i);
    }
  }

  return keys;
}

}  // namespace braque
