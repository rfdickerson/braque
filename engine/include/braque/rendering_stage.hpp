//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef RENDERING_STAGE_HPP
#define RENDERING_STAGE_HPP

#include <memory>

#include <vulkan/vulkan.hpp>

namespace braque {

class Engine;
    class Image;

class RenderingStage {
public:
    explicit RenderingStage(Engine& engine);
    ~RenderingStage();

    // make sure copy and move are deleted
    RenderingStage(const RenderingStage&) = delete;
    auto operator=(const RenderingStage&) -> RenderingStage& = delete;
    RenderingStage(RenderingStage&&) = delete;
    auto operator=(RenderingStage&&) -> RenderingStage& = delete;

    [[nodiscard]] auto getDescriptorPool() const -> vk::DescriptorPool { return descriptorPool; }

    void begin(vk::CommandBuffer buffer);
    void beginRenderingPass(vk::CommandBuffer buffer);
    void prepareImageForColorAttachment(vk::CommandBuffer buffer);
    void prepareImageForDisplay(vk::CommandBuffer buffer);
    void endRenderingPass(vk::CommandBuffer buffer);
    void end(vk::CommandBuffer buffer);

    // void render();

private:
    Engine& engine;

    vk::DescriptorPool descriptorPool;
    std::unique_ptr<Image> offscreenImage;

    void createDescriptorPool();

};

} // braque

#endif //RENDERING_STAGE_HPP
