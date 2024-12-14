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

private:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::Device device;

    void createInstance();
    void createPhysicalDevice();
    void createLogicalDevice();
};

} // braque

#endif //RENDERER_HPP
