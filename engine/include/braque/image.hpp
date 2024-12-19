//
// Created by rfdic on 12/17/2024.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "engine.hpp"
#include "memory_allocator.hpp"

namespace braque {

struct SyncBarriers {
    vk::PipelineStageFlags2 srcStage {};
    vk::AccessFlags2 srcAccess {};
    vk::PipelineStageFlags2 dstStage {};
    vk::AccessFlags2 dstAccess {};
};

class Image {
public:
    Image(Engine& engine, vk::Extent3D extent, vk::Format format);

    void transitionLayout(vk::ImageLayout newLayout, vk::CommandBuffer commandBuffer, SyncBarriers barriers = {});

    ~Image();

private:
    Engine& engine;
    AllocatedImage allocatedImage;
    vk::ImageView imageView;
    vk::Extent3D extent;
    vk::Format format;
    vk::ImageLayout layout;

    void allocateImage();
    void createImageView();
};

} // braque

#endif //IMAGE_HPP
