//
// Created by rfdic on 12/17/2024.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "engine.h"
#include "memory_allocator.h"

namespace braque
{

  struct SyncBarriers
  {
    vk::PipelineStageFlags2 srcStage;
    vk::AccessFlags2        srcAccess;
    vk::PipelineStageFlags2 dstStage;
    vk::AccessFlags2        dstAccess;
  };

  class Image
  {
  public:
    Image( Engine & engine, vk::Extent3D extent, vk::Format format );
    ~Image();

    // declare the copy constructor
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;

    // make sure copy and move are deleted
    Image( const Image & )                     = delete;
    Image& operator=( const Image & ) = delete;


    [[nodiscard]] auto GetImage() const -> vk::Image {
      return image_;
    }

    [[nodiscard]] auto GetImageView() const -> vk::ImageView
    {
      return image_view_;
    }

    [[nodiscard]] auto GetExtent() const -> vk::Extent3D
    {
      return extent_;
    }

    [[nodiscard]] auto GetFormat() const -> vk::Format
    {
      return format;
    }

    [[nodiscard]] auto GetLayout() const -> vk::ImageLayout
    {
      return layout_;
    }

    void TransitionLayout( vk::ImageLayout newLayout, vk::CommandBuffer commandBuffer, const SyncBarriers & barriers = {} );

    void BlitImage(vk::CommandBuffer buffer, const Image & destImage) const;

  private:
    Engine &        engine_;

    vk::Image image_;
    VmaAllocation   allocation_;
    vk::ImageView   image_view_;
    vk::Extent3D    extent_;
    vk::Format      format;
    vk::ImageLayout layout_;

    void allocateImage();
    void createImageView();
  };

}  // namespace braque

#endif  // IMAGE_HPP
