//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/renderer.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace braque {

Renderer::Renderer()
    : instance_(createInstance()),
      m_physicalDevice(createPhysicalDevice(instance_)),
      m_device(createLogicalDevice(m_physicalDevice)),
      m_graphicsQueue(createGraphicsQueue(m_device, 0)),
      command_pool_(CreateCommandPool(m_device, 0)),
      graphicsQueueFamilyIndex(0) {
  spdlog::info("Created renderer");
}

Renderer::~Renderer() {

  // destroy the command pool
  m_device.destroyCommandPool(command_pool_);

  // destroy the device
  m_device.destroy();

  spdlog::info("Destroying renderer");

  instance_.destroy();
}

vk::Instance Renderer::createInstance() {
  VULKAN_HPP_DEFAULT_DISPATCHER.init();

  vk::ApplicationInfo applicationInfo;
  applicationInfo.setPApplicationName("Braque");
  applicationInfo.setApplicationVersion(1);
  applicationInfo.setPEngineName("Braque");
  applicationInfo.setEngineVersion(1);
  applicationInfo.setApiVersion(VK_API_VERSION_1_2);

  const auto extensions = getInstanceExtensions();
  const auto flags = getInstanceFlags();

  vk::InstanceCreateInfo instanceInfo;
  instanceInfo.setEnabledExtensionCount(
      static_cast<uint32_t>(extensions.size()));
  instanceInfo.setPpEnabledExtensionNames(extensions.data());
  instanceInfo.setPApplicationInfo(&applicationInfo);
  instanceInfo.setFlags(flags);

  auto instance = vk::createInstance(instanceInfo);

  if (!instance) {
    spdlog::error("Failed to create Vulkan instance");
    throw std::runtime_error("Failed to create Vulkan instance");
  }

  VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

  spdlog::info("Vulkan instance created");

  return instance;
}

vk::PhysicalDevice Renderer::createPhysicalDevice(vk::Instance instance) {
  // get the physical device
  auto physicalDevices = instance.enumeratePhysicalDevices();
  if (physicalDevices.empty()) {
    spdlog::error("No physical devices found");
    throw std::runtime_error("No physical devices found");
  }

  auto physicalDevice = physicalDevices[0];
  spdlog::info("Physical device found");

  // print the physical device properties
  auto properties = physicalDevice.getProperties();
  spdlog::info("Physical device properties:");
  spdlog::info("  Name: {}", std::string(properties.deviceName.data()));
  spdlog::info("  Type: {}", vk::to_string(properties.deviceType));

  // double check it supports synchronization 2 and dynamic rendering
  auto features = physicalDevice.getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceDynamicRenderingFeatures,
      vk::PhysicalDeviceSynchronization2FeaturesKHR>();
  if (features.get<vk::PhysicalDeviceDynamicRenderingFeatures>()
          .dynamicRendering == vk::False) {
    spdlog::error("Physical device does not support dynamic rendering");
    throw std::runtime_error(
        "Physical device does not support dynamic rendering");
  }

  if (features.get<vk::PhysicalDeviceSynchronization2FeaturesKHR>()
          .synchronization2 == vk::False) {
    spdlog::error("Physical device does not support synchronization 2");
    throw std::runtime_error(
        "Physical device does not support synchronization 2");
  }

  return physicalDevice;
}

vk::Device Renderer::createLogicalDevice(vk::PhysicalDevice physicalDevice) {
  // create the logical device
  vk::DeviceQueueCreateInfo queueCreateInfo;
  queueCreateInfo.setQueueFamilyIndex(0);
  queueCreateInfo.setQueueCount(1);
  constexpr auto queuePriority = 1.0F;
  queueCreateInfo.setPQueuePriorities(&queuePriority);

  vk::DeviceCreateInfo deviceCreateInfo;
  deviceCreateInfo.setQueueCreateInfoCount(1);
  deviceCreateInfo.setPQueueCreateInfos(&queueCreateInfo);

  auto deviceExtensions = getDeviceExtensions();

  deviceCreateInfo.setEnabledExtensionCount(
      static_cast<uint32_t>(deviceExtensions.size()));
  deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());

  vk::PhysicalDeviceFeatures enabledFeatures{};
  enabledFeatures.setSamplerAnisotropy(true);
  deviceCreateInfo.setPEnabledFeatures(&enabledFeatures);

  // dynamic rendering features
  vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
  dynamicRenderingFeatures.setDynamicRendering(vk::True);

  // synchronization 2 features
  vk::PhysicalDeviceSynchronization2FeaturesKHR synchronization2Features;
  synchronization2Features.setSynchronization2(vk::True);
  synchronization2Features.setPNext(&dynamicRenderingFeatures);

  // float16 int8 features
  vk::PhysicalDeviceFloat16Int8FeaturesKHR float16Int8Features;
  float16Int8Features.setShaderFloat16(vk::True);
  float16Int8Features.setShaderInt8(vk::True);
  float16Int8Features.setPNext(&synchronization2Features);

  deviceCreateInfo.setPNext(&float16Int8Features);

  const auto device = physicalDevice.createDevice(deviceCreateInfo);

  if (!device) {
    spdlog::error("Failed to create logical device");
    throw std::runtime_error("Failed to create logical device");
  }

  spdlog::info("Logical device created");

  // make it not need to look up the dynamic dispatcher each time
  VULKAN_HPP_DEFAULT_DISPATCHER.init(device);

  return device;
}

vk::Queue Renderer::createGraphicsQueue(vk::Device device,
                                        uint32_t graphicsQueueFamilyIndex) {
  return device.getQueue(graphicsQueueFamilyIndex, 0);
}

void Renderer::waitIdle() const {
  m_device.waitIdle();
}

auto Renderer::getInstanceExtensions() -> std::vector<char const*> {
  uint32_t glfwExtensionCount = 0;
  auto const* glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if __APPLE__
  extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);
#endif

  return extensions;
}

auto Renderer::getDeviceExtensions() -> std::vector<const char*> {
  std::vector deviceExtensions = {
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
  };

#ifdef __APPLE__
  deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

  return deviceExtensions;
}

auto Renderer::getInstanceFlags() -> vk::InstanceCreateFlags {
  vk::InstanceCreateFlags flags{};

#ifdef __APPLE__
  flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

  return flags;
}

auto Renderer::CreateCommandBuffer() const -> vk::CommandBuffer {
  vk::CommandBufferAllocateInfo allocateInfo;
  allocateInfo.setCommandPool(command_pool_);
  allocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
  allocateInfo.setCommandBufferCount(1);

  return m_device.allocateCommandBuffers(allocateInfo)[0];
}

vk::CommandPool Renderer::CreateCommandPool(vk::Device device,
                                            uint32_t graphicsQueueFamilyIndex) {
  vk::CommandPoolCreateInfo poolInfo{};
  poolInfo.setQueueFamilyIndex(graphicsQueueFamilyIndex);
  poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

  auto command_pool = device.createCommandPool(poolInfo);

  return command_pool;
}

void Renderer::SubmitAndWait(vk::CommandBuffer cmd) const {
  vk::SubmitInfo submitInfo {};
  submitInfo.setCommandBufferCount(1);
  submitInfo.setPCommandBuffers(&cmd);

  vk::FenceCreateInfo fenceInfo;
  vk::Fence fence = m_device.createFence(fenceInfo);

  // Submit the command buffer
  auto submit_result = m_graphicsQueue.submit(1, &submitInfo, fence);

  if (submit_result != vk::Result::eSuccess) {
    spdlog::error("Failed to submit command buffer");
    throw std::runtime_error("Failed to submit command buffer");
  }

  // Wait for the fence to be signaled
  vk::Result result = m_device.waitForFences(
      1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
  if (result != vk::Result::eSuccess) {
    spdlog::error("Failed to wait for fence");
    throw std::runtime_error("Failed to wait for fence");
  }

  // Clean up
  m_device.destroyFence(fence);
}

}  // namespace braque