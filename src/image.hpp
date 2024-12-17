//
// Created by rfdic on 12/17/2024.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "engine.hpp"
#include "memory_allocator.hpp"

namespace braque {

class Image {
public:
    Image(Engine& engine, vk::Extent3D extent, vk::Format format);
    ~Image();

private:
    Engine& engine;
    AllocatedImage allocatedImage;
    vk::ImageView imageView;
    vk::Format format;
    vk::Extent3D extent;

    void allocateImage();
    void createImageView();
};

} // braque

#endif //IMAGE_HPP
