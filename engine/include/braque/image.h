//
// Created by rfdic on 12/17/2024.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "vulkan/vulkan.hpp"
#include "vk_mem_alloc.h"

#include "engine_context.h"

namespace braque {

struct SyncBarriers {
  vk::PipelineStageFlags2 srcStage;
  vk::AccessFlags2 srcAccess;
  vk::PipelineStageFlags2 dstStage;
  vk::AccessFlags2 dstAccess;
};

struct ImageConfig {
  vk::Extent3D extent = vk::Extent3D(0, 0, 0);
  vk::Format format = vk::Format::eUndefined;
  vk::ImageLayout layout = vk::ImageLayout::eUndefined;
  vk::ImageUsageFlags usage;
  vk::ImageType imageType = vk::ImageType::e2D;
  uint32_t mipLevels = 1;
  uint32_t arrayLayers = 1;
  uint32_t samples = 1;
};

class Image {

 public:
  Image(EngineContext& engine, const vk::ImageCreateInfo& createInfo,
        const VmaAllocationCreateInfo& allocInfo);
  Image(EngineContext& engine, vk::Extent3D extent, vk::Format format);

  // Image constructor with existing image
  Image(EngineContext& engine, vk::Image image, vk::Format format, vk::ImageLayout layout);

  Image(EngineContext& engine, const ImageConfig& config);

  ~Image();

  // declare the copy constructor
  Image(Image&& other) noexcept;
  Image& operator=(Image&& other) noexcept;

  // make sure copy and move are deleted
  Image(const Image&) = delete;
  Image& operator=(const Image&) = delete;

  [[nodiscard]] auto GetImage() const -> vk::Image { return image_; }

  [[nodiscard]] auto GetImageView() const -> vk::ImageView {
    return image_view_;
  }

  [[nodiscard]] auto GetExtent() const -> vk::Extent3D { return extent_; }

  [[nodiscard]] auto GetFormat() const -> vk::Format { return format; }

  [[nodiscard]] auto GetLayout() const -> vk::ImageLayout { return layout_; }

  void TransitionLayout(vk::ImageLayout newLayout,
                        vk::CommandBuffer commandBuffer,
                        const SyncBarriers& barriers = {},
                        uint32_t mipLevels = 1);

  void BlitImage(vk::CommandBuffer buffer, const Image& destImage) const;

 private:
  EngineContext& engine_;

  vk::Image image_;
  VmaAllocation allocation_;
  vk::ImageView image_view_;
  vk::Extent3D extent_;
  vk::Format format;
  vk::ImageLayout layout_;

  uint32_t mip_levels_;

  void allocateImage();
  void createImageView();

  static auto CreateImageInfo(const ImageConfig& config) -> vk::ImageCreateInfo;
  static auto GetSampleCount(uint32_t samples) -> vk::SampleCountFlagBits;
  static auto GetAllocationInfo() -> VmaAllocationCreateInfo;
};

}  // namespace braque

#endif  // IMAGE_HPP
