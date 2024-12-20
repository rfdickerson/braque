//
// Created by rfdic on 12/17/2024.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "engine.hpp"
#include "memory_allocator.hpp"

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

    // remove copy and move
    Image( const Image & )                     = delete;
    auto operator=( const Image & ) -> Image & = delete;
    Image( Image && )                          = delete;
    auto operator=( Image && ) -> Image &      = delete;

    [[nodiscard]] auto getImageView() const -> vk::ImageView
    {
      return imageView;
    }

    [[nodiscard]] auto getExtent() const -> vk::Extent3D
    {
      return extent;
    }

    [[nodiscard]] auto getFormat() const -> vk::Format
    {
      return format;
    }

    [[nodiscard]] auto getLayout() const -> vk::ImageLayout
    {
      return layout;
    }

    void transitionLayout( vk::ImageLayout newLayout, vk::CommandBuffer commandBuffer, const SyncBarriers & barriers = {} );

  private:
    Engine &        engine;
    AllocatedImage  allocatedImage;
    vk::ImageView   imageView;
    vk::Extent3D    extent;
    vk::Format      format;
    vk::ImageLayout layout;

    void allocateImage();
    void createImageView();
  };

}  // namespace braque

#endif  // IMAGE_HPP
