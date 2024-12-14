//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "renderer.hpp"

#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace braque {

Renderer::Renderer() {
    spdlog::info("Creating renderer");

    createInstance();
    createPhysicalDevice();
    createLogicalDevice();
}

Renderer::~Renderer() {
    spdlog::info("Destroying renderer");

    instance.destroy();
}

void Renderer::createInstance() {

    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    // initialize the instance
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName("Braque");
    applicationInfo.setApplicationVersion(1);
    applicationInfo.setPEngineName("Braque");
    applicationInfo.setEngineVersion(1);
    applicationInfo.setApiVersion(VK_API_VERSION_1_2);

    // create a vector of extensions and add portability extension
    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    instanceInfo.setPpEnabledExtensionNames(extensions.data());
    instanceInfo.setPApplicationInfo(&applicationInfo);
    instanceInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);

    instance = vk::createInstance(instanceInfo);

    if (!instance) {
        spdlog::error("Failed to create Vulkan instance");
        throw std::runtime_error("Failed to create Vulkan instance");
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

    spdlog::info("Vulkan instance created");


}

void Renderer::createPhysicalDevice() {
    // get the physical device
    auto physicalDevices = instance.enumeratePhysicalDevices();
    if (physicalDevices.empty()) {
        spdlog::error("No physical devices found");
        throw std::runtime_error("No physical devices found");
    }

    physicalDevice = physicalDevices[0];
    spdlog::info("Physical device found");

    // print the physical device properties
    auto properties = physicalDevice.getProperties();
    spdlog::info("Physical device properties:");
    spdlog::info("  Name: {}", std::string(properties.deviceName));
    spdlog::info("  Type: {}", vk::to_string(properties.deviceType));

}

void Renderer::createLogicalDevice() {

    // create the logical device
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(0);
    queueCreateInfo.setQueueCount(1);
    float queuePriority = 1.0f;
    queueCreateInfo.setPQueuePriorities(&queuePriority);

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setQueueCreateInfoCount(1);
    deviceCreateInfo.setPQueueCreateInfos(&queueCreateInfo);

    std::vector<const char *> deviceExtensions = {VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME};
    deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
    deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());

    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
    dynamicRenderingFeatures.setDynamicRendering(true);
    deviceCreateInfo.setPNext(&dynamicRenderingFeatures);

    device = physicalDevice.createDevice(deviceCreateInfo);

    if (!device) {
        spdlog::error("Failed to create logical device");
        throw std::runtime_error("Failed to create logical device");
    }

    spdlog::info("Logical device created");
}



} // braque