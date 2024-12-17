//
// Created by rfdic on 12/17/2024.
//

#include "image.hpp"

#include "renderer.hpp"

#include <spdlog/spdlog.h>

namespace braque {

    Image::Image(Engine& engine, vk::Extent3D extent, vk::Format format): engine(engine), extent(extent), format(format) {

        allocateImage();
        createImageView();

    }

    Image::~Image() {

        // destroy image view
        engine.getRenderer().getDevice().destroyImageView(imageView);

        engine.getMemoryAllocator().destroyImage(allocatedImage);
    }

    void Image::allocateImage()
    {
        vk::ImageCreateInfo createInfo;
        createInfo.setImageType(vk::ImageType::e2D);
        createInfo.setExtent(extent);
        createInfo.setMipLevels(1);
        createInfo.setArrayLayers(1);
        createInfo.setFormat(format);
        createInfo.setTiling(vk::ImageTiling::eOptimal);
        createInfo.setInitialLayout(vk::ImageLayout::eUndefined);
        createInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);
        createInfo.setSamples(vk::SampleCountFlagBits::e4);
        createInfo.setSharingMode(vk::SharingMode::eExclusive);

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        allocatedImage = engine.getMemoryAllocator().createImage(createInfo, allocInfo);

    }

    void Image::createImageView()
    {
        vk::ImageViewCreateInfo createInfo;
        createInfo.setImage(allocatedImage.image);
        createInfo.setViewType(vk::ImageViewType::e2D);
        createInfo.setFormat(format);
        createInfo.setComponents({vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA});
        createInfo.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        imageView = engine.getRenderer().getDevice().createImageView(createInfo);

        spdlog::info("Created image view");
    }




} // braque