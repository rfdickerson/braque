//
// Created by rfdic on 12/17/2024.
//

#include "braque/image.h"

#include "braque/memory_allocator.h"
#include "braque/renderer.h"

#include <spdlog/spdlog.h>

namespace braque {

Image::Image(EngineContext& engine, const vk::ImageCreateInfo& createInfo,
             const VmaAllocationCreateInfo& allocInfo)
    : engine_(engine),
      allocation_(nullptr),
      image_view_(nullptr),
      format(createInfo.format),
      layout_(createInfo.initialLayout),
      mip_levels_(createInfo.mipLevels) {

  auto [image, allocation] =
      engine_.getMemoryAllocator().createImage(createInfo, allocInfo);

  allocation_ = allocation;
  image_ = image;

  createImageView();
}

Image::Image(EngineContext& engine, vk::Extent3D extent, vk::Format format)
    : engine_(engine),
      allocation_(nullptr),
      image_view_(nullptr),
      extent_(extent),
      format(format),
      layout_(vk::ImageLayout::eUndefined),
      mip_levels_(1) {
  allocateImage();
  createImageView();

  spdlog::info("Created image");
}

Image::Image(EngineContext& engine, vk::Image image, vk::Format format,
             vk::ImageLayout layout, vk::Extent3D extent)
    : engine_(engine),
      image_(image),
      allocation_(nullptr),
      extent_(extent),
      format(format),
      layout_(layout),
      mip_levels_(1) {
  createImageView();
}

Image::Image(EngineContext& engine, const ImageConfig& config)
    : engine_(engine),
      extent_(config.extent),
      format(config.format),
      layout_(config.layout),
      mip_levels_(config.mipLevels) {

  auto imageInfo = CreateImageInfo(config);
  auto allocInfo = GetAllocationInfo();

  auto [image, allocation] =
      engine_.getMemoryAllocator().createImage(imageInfo, allocInfo);

  allocation_ = allocation;
  image_ = image;
  createImageView();
}

vk::SampleCountFlagBits Image::GetSampleCount(uint32_t samples) {
  switch (samples) {
    case 1:
      return vk::SampleCountFlagBits::e1;
    case 2:
      return vk::SampleCountFlagBits::e2;
    case 4:
      return vk::SampleCountFlagBits::e4;
    case 8:
      return vk::SampleCountFlagBits::e8;
    default:
      return vk::SampleCountFlagBits::e1;
  }
}

vk::ImageCreateInfo Image::CreateImageInfo(const ImageConfig& config) {
  vk::ImageCreateInfo imageInfo{};
  imageInfo.imageType = config.imageType;
  imageInfo.extent = config.extent;
  imageInfo.mipLevels = config.mipLevels;
  imageInfo.arrayLayers = config.arrayLayers;
  imageInfo.format = config.format;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.initialLayout = config.layout;
  imageInfo.usage = config.usage;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  imageInfo.samples = GetSampleCount(config.samples);
  return imageInfo;
}

VmaAllocationCreateInfo Image::GetAllocationInfo() {
  VmaAllocationCreateInfo allocInfo{};

  allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  return allocInfo;
}

Image::~Image() {
  if (image_view_) {
    // destroy image view
    engine_.getRenderer().getDevice().destroyImageView(image_view_);
    image_view_ = nullptr;
    spdlog::info("Destroyed image view");
  }

  if (image_ != nullptr && allocation_ != nullptr) {
    engine_.getMemoryAllocator().destroyImage(image_, allocation_);
    image_ = nullptr;
    allocation_ = nullptr;
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

Image& Image::operator=(Image&& other) noexcept {
  if (this != &other) {
    // First, clean up any existing resources
    if (image_view_) {
      engine_.getRenderer().getDevice().destroyImageView(image_view_);
      image_view_ = nullptr;
    }
    if (image_ && allocation_) {
      engine_.getMemoryAllocator().destroyImage(image_, allocation_);
      image_ = nullptr;
      allocation_ = nullptr;
    }
  }
  return *this;
}

void Image::allocateImage() {
  vk::ImageCreateInfo createInfo{};
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
    createInfo.setUsage(vk::ImageUsageFlagBits::eColorAttachment |
                        vk::ImageUsageFlagBits::eTransferDst |
                        vk::ImageUsageFlagBits::eSampled);
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
  vk::ImageViewCreateInfo createInfo{};
  createInfo.setImage(image_);
  createInfo.setViewType(vk::ImageViewType::e2D);
  createInfo.setFormat(format);

  if (format == vk::Format::eD32Sfloat) {
    createInfo.setSubresourceRange(
        {vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1});
  } else {
    createInfo.setSubresourceRange(
        {vk::ImageAspectFlagBits::eColor, 0, mip_levels_, 0, 1});
    createInfo.setComponents(
        {vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
         vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA});
  }

  image_view_ = engine_.getRenderer().getDevice().createImageView(createInfo);

  spdlog::info("Created image view with {} mip levels", mip_levels_);
}

void Image::TransitionLayout(const vk::ImageLayout newLayout,
                             const vk::CommandBuffer commandBuffer,
                             const SyncBarriers& barriers, uint32_t mipLevels) {
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
  barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;  // use all mip levels
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

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

  vk::ImageBlit region;
  region.srcOffsets[0] = vk::Offset3D{0, 0, 0};
  region.srcOffsets[1] = vk::Offset3D{static_cast<int32_t>(extent_.width),
                                      static_cast<int32_t>(extent_.height), 1};
  region.dstOffsets[0] = vk::Offset3D{0, 0, 0};
  region.dstOffsets[1] =
      vk::Offset3D{static_cast<int32_t>(destImage.GetExtent().width),
                   static_cast<int32_t>(destImage.GetExtent().height), 1};
  region.srcSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1};
  region.dstSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1};

  // do the blit
  buffer.blitImage(image_,                 // srcImage
                   layout_,                // srcImageLayout
                   destImage.GetImage(),   // dstImage
                   destImage.GetLayout(),  // dstImageLayout
                   1,                      // regionCount
                   &region,                // pRegions
                   vk::Filter::eLinear     // filter
  );
}

void Image::ResolveImage(vk::CommandBuffer buffer,
                         const Image& destImage) const {

  vk::ImageResolve resolveRegion;
  resolveRegion.srcSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1};
  resolveRegion.srcOffset = vk::Offset3D{0, 0, 0};
  resolveRegion.dstSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1};
  resolveRegion.dstOffset = vk::Offset3D{0, 0, 0};
  resolveRegion.extent = extent_;

  buffer.resolveImage(image_,                 // srcImage
                      layout_,                // srcImageLayout
                      destImage.GetImage(),   // dstImage
                      destImage.GetLayout(),  // dstImageLayout
                      1,                      // regionCount
                      &resolveRegion          // pRegions
  );
}

}  // namespace braque