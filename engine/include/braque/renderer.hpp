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

    // make sure copy and move are deleted
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void waitIdle();

    vk::Instance getInstance() const { return m_instance; }
    vk::Device getDevice() const { return m_device; }
    vk::PhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    vk::Queue getGraphicsQueue() const { return m_graphicsQueue; }
    uint32_t getGraphicsQueueFamilyIndex() const { return graphicsQueueFamilyIndex; }


private:
    vk::Instance m_instance;
    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_device;
    vk::Queue m_graphicsQueue;

    uint32_t graphicsQueueFamilyIndex;

    void createInstance();
    void createPhysicalDevice();
    void createLogicalDevice();

    static std::vector<const char*> getInstanceExtensions();
    static std::vector<const char*> getDeviceExtensions();
    static vk::InstanceCreateFlags getInstanceFlags();
};

} // braque

#endif //RENDERER_HPP
