//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "vulkan/vulkan.hpp"

namespace braque {

using VulkanString = const char*;

class Renderer {
 public:
  Renderer();
  ~Renderer();

  // make sure copy and move are deleted
  Renderer(const Renderer&) = delete;
  auto operator=(const Renderer&) -> Renderer& = delete;
  Renderer(Renderer&&) = delete;
  auto operator=(Renderer&&) -> Renderer& = delete;

  void waitIdle() const;

  [[nodiscard]] auto getInstance() const -> vk::Instance { return instance_; }

  [[nodiscard]] auto getDevice() const -> vk::Device { return m_device; }

  [[nodiscard]] auto getPhysicalDevice() const -> vk::PhysicalDevice {
    return m_physicalDevice;
  }

  [[nodiscard]] auto getGraphicsQueue() const -> vk::Queue {
    return m_graphicsQueue;
  }

  [[nodiscard]] auto getGraphicsQueueFamilyIndex() const -> uint32_t {
    return graphicsQueueFamilyIndex;
  }

  [[nodiscard]] auto CreateCommandBuffer() const -> vk::CommandBuffer;

  void SubmitAndWait(vk::CommandBuffer cmd) const;

 private:
  vk::Instance instance_;
  vk::PhysicalDevice m_physicalDevice;
  vk::Device m_device;
  vk::Queue m_graphicsQueue;

  // used for creating command buffers
  vk::CommandPool command_pool_;

  uint32_t graphicsQueueFamilyIndex;

  static vk::Instance createInstance();
  static vk::PhysicalDevice createPhysicalDevice(vk::Instance instance);
  static vk::Device createLogicalDevice(vk::PhysicalDevice physicalDevice);
  static vk::Queue createGraphicsQueue(vk::Device device, uint32_t graphicsQueueFamilyIndex);
  static vk::CommandPool CreateCommandPool(vk::Device device, uint32_t graphicsQueueFamilyIndex);

  static auto getInstanceExtensions() -> std::vector<VulkanString>;
  static auto getDeviceExtensions() -> std::vector<VulkanString>;
  static auto getInstanceFlags() -> vk::InstanceCreateFlags;
};

}  // namespace braque

#endif  // RENDERER_HPP
