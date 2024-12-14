//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

namespace braque {

class Renderer {
public:
    Renderer();
    ~Renderer();

    vk::Instance getInstance() const { return instance; }
    vk::Device getDevice() const { return device; }
    vk::PhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    vk::Queue getGraphicsQueue() const { return graphicsQueue; }
    uint32_t getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }


private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;
    vk::Queue graphicsQueue;

    uint32_t graphicsQueueFamilyIndex;

    void createInstance();
    void createPhysicalDevice();
    void createLogicalDevice();
};

} // braque

#endif //RENDERER_HPP
