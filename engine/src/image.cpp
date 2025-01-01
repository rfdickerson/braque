//
// Created by rfdic on 12/17/2024.
//

#include "braque/image.h"

#include "braque/renderer.h"

#include <spdlog/spdlog.h>

namespace braque {

Image::Image(Engine& engine, vk::Extent3D extent, vk::Format format)
    : engine_(engine),
      extent_(extent),
      format(format),
      layout_(vk::ImageLayout::eUndefined) {
  allocateImage();
  createImageView();

  spdlog::info("Created image");
}

Image::~Image() {
  if (image_view_) {
    // destroy image view
    engine_.getRenderer().getDevice().destroyImageView(image_view_);
    spdlog::info("Destroyed image view");
  }

  if (image_) {
    auto allocator = engine_.getMemoryAllocator().getAllocator();

    vmaDestroyImage(allocator, image_, allocation_);
    spdlog::info("Destroyed image memory");
  }
}

Image::Image(Image&& other) noexcept
    : engine_(other.engine_),
      image_(other.GetImage()),
      allocation_(other.allocation_),
      image_view_(other.GetImageView()),
      extent_(other.extent_),
      format(other.format),
      layout_(other.layout_) {
  other.image_ = nullptr;
  other.image_view_ = nullptr;
  other.allocation_ = nullptr;
}

void Image::allocateImage() {
  vk::ImageCreateInfo createInfo;
  createInfo.setImageType(vk::ImageType::e2D);
  createInfo.setExtent(extent_);
  createInfo.setMipLevels(1);
  createInfo.setArrayLayers(1);
  createInfo.setFormat(format);
  createInfo.setTiling(vk::ImageTiling::eOptimal);
  createInfo.setInitialLayout(vk::ImageLayout::eUndefined);

  if (format == vk::Format::eD32Sfloat) {
    createInfo.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment |
                        vk::ImageUsageFlagBits::eTransferSrc);
  } else {
    createInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment);
  }

  createInfo.setSamples(vk::SampleCountFlagBits::e1);
  createInfo.setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo allocInfo{};
  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  auto [image, allocation] =
      engine_.getMemoryAllocator().createImage(createInfo, allocInfo);

  allocation_ = allocation;
  image_ = image;
}

void Image::createImageView() {
  vk::ImageViewCreateInfo createInfo;
  createInfo.setImage(image_);
  createInfo.setViewType(vk::ImageViewType::e2D);
  createInfo.setFormat(format);

  if (format == vk::Format::eD32Sfloat) {
    createInfo.setSubresourceRange({vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});
  } else {
    createInfo.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    createInfo.setComponents({vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
                            vk::ComponentSwizzle::eB,
                            vk::ComponentSwizzle::eA});
  }

  image_view_ = engine_.getRenderer().getDevice().createImageView(createInfo);

  spdlog::info("Created image view");
}

void Image::TransitionLayout(const vk::ImageLayout newLayout,
                             const vk::CommandBuffer commandBuffer,
                             const SyncBarriers& barriers) {
  // create a barrier to transition the image layout
  // use the pipelineBarrier2KHRs to use synchronization2
  vk::ImageMemoryBarrier2KHR barrier;
  barrier.srcStageMask = barriers.srcStage;
  barrier.srcAccessMask = barriers.srcAccess;
  barrier.dstStageMask = barriers.dstStage;
  barrier.dstAccessMask = barriers.dstAccess;
  barrier.oldLayout = layout_;
  barrier.newLayout = newLayout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image_;
  barrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

  vk::DependencyInfoKHR dependencyInfo;
  dependencyInfo.imageMemoryBarrierCount = 1;
  dependencyInfo.pImageMemoryBarriers = &barrier;

  commandBuffer.pipelineBarrier2KHR(dependencyInfo);

  layout_ = newLayout;
}

void Image::BlitImage(const vk::CommandBuffer buffer,
                      const Image& destImage) const {
  // check that source is in a transfer source layout
  if (layout_ != vk::ImageLayout::eTransferSrcOptimal) {
    spdlog::error("Source image is not in transfer source optimal layout");
    throw std::runtime_error(
        "Source image is not in transfer source optimal layout");
  }

  // check that destination is in a transfer destination layout
  if (destImage.GetLayout() != vk::ImageLayout::eTransferDstOptimal) {
    spdlog::error(
        "Destination image is not in transfer destination optimal layout");
    throw std::runtime_error(
        "Destination image is not in transfer destination optimal layout");
  }

  vk::ImageBlit2KHR regions;
  regions.srcOffsets[0] = vk::Offset3D{0, 0, 0};
  regions.dstOffsets[0] = vk::Offset3D{0, 0, 0};
  regions.srcOffsets[1] = vk::Offset3D{static_cast<int32_t>(extent_.width),
                                       static_cast<int32_t>(extent_.height), 1};
  regions.dstOffsets[1] =
      vk::Offset3D{static_cast<int32_t>(destImage.GetExtent().width),
                   static_cast<int32_t>(destImage.GetExtent().height), 1};
  regions.srcSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1};

  // do the blit
  vk::BlitImageInfo2KHR blitInfo;
  blitInfo.setSrcImage(image_);
  blitInfo.setSrcImageLayout(layout_);
  blitInfo.setDstImage(destImage.GetImage());
  blitInfo.setRegions(regions);
  blitInfo.setFilter(vk::Filter::eLinear);

  buffer.blitImage2KHR(blitInfo);
}

}  // namespace braque