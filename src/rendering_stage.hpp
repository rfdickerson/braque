//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef RENDERING_STAGE_HPP
#define RENDERING_STAGE_HPP

#include <vulkan/vulkan.hpp>

namespace braque {

class Renderer;
class Swapchain;

class RenderingStage {
public:
    RenderingStage(Renderer &renderer, Swapchain &swapchain);
    ~RenderingStage();

    vk::DescriptorPool getDescriptorPool() const { return descriptorPool; }

    void begin(vk::CommandBuffer buffer);
    void beginRenderingPass(vk::CommandBuffer buffer);
    void prepareImageForColorAttachment(vk::CommandBuffer buffer);
    void prepareImageForDisplay(vk::CommandBuffer buffer);
    void endRenderingPass(vk::CommandBuffer buffer);
    void end(vk::CommandBuffer buffer);

    // void render();

private:
    Renderer& renderer;
    Swapchain& swapchain;

    vk::DescriptorPool descriptorPool;

    void createDescriptorPool();

};

} // braque

#endif //RENDERING_STAGE_HPP
