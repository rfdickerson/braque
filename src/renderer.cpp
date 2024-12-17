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

    // destroy the device
    device.destroy();
    

    spdlog::info("Destroying renderer");

    instance.destroy();
}

void Renderer::createInstance() {

    VULKAN_HPP_DEFAULT_DISPATCHER.init();

    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName("Braque");
    applicationInfo.setApplicationVersion(1);
    applicationInfo.setPEngineName("Braque");
    applicationInfo.setEngineVersion(1);
    applicationInfo.setApiVersion(VK_API_VERSION_1_2);

    auto extensions = getInstanceExtensions();
    auto flags = getInstanceFlags();

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    instanceInfo.setPpEnabledExtensionNames(extensions.data());
    instanceInfo.setPApplicationInfo(&applicationInfo);
    instanceInfo.setFlags(flags);

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

    // double check it supports syncronization 2 and dynamic rendering
    auto features = physicalDevice.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceDynamicRenderingFeatures, vk::PhysicalDeviceSynchronization2FeaturesKHR>();
    if (!features.get<vk::PhysicalDeviceDynamicRenderingFeatures>().dynamicRendering) {
        spdlog::error("Physical device does not support dynamic rendering");
        throw std::runtime_error("Physical device does not support dynamic rendering");
    }

    if (!features.get<vk::PhysicalDeviceSynchronization2FeaturesKHR>().synchronization2) {
        spdlog::error("Physical device does not support synchronization 2");
        throw std::runtime_error("Physical device does not support synchronization 2");
    }


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

    auto deviceExtensions = getDeviceExtensions();

    deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(deviceExtensions.size()));
    deviceCreateInfo.setPpEnabledExtensionNames(deviceExtensions.data());

    vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
    dynamicRenderingFeatures.setDynamicRendering(true);


    // synchronization 2 features
    vk::PhysicalDeviceSynchronization2FeaturesKHR synchronization2Features;
    synchronization2Features.setSynchronization2(true);
    synchronization2Features.setPNext(&dynamicRenderingFeatures);

    deviceCreateInfo.setPNext(&synchronization2Features);

    device = physicalDevice.createDevice(deviceCreateInfo);

    if (!device) {
        spdlog::error("Failed to create logical device");
        throw std::runtime_error("Failed to create logical device");
    }

    spdlog::info("Logical device created");

    // set the queue
    graphicsQueue = device.getQueue(0, 0);
    graphicsQueueFamilyIndex = 0;
}


void Renderer::waitIdle() {
    device.waitIdle();
}

std::vector<char const *> Renderer::getInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if __APPLE__
    extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);
#endif

    return extensions;
}

std::vector<const char *> Renderer::getDeviceExtensions() {
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

vk::InstanceCreateFlags Renderer::getInstanceFlags() {
    vk::InstanceCreateFlags flags {};

#ifdef __APPLE__
    flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

    return flags;
}



} // braque