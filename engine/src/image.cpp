//
// Created by rfdic on 12/17/2024.
//

#include "braque/image.hpp"

#include "braque/renderer.hpp"

#include <spdlog/spdlog.h>

namespace braque
{

  Image::Image( Engine & engine, vk::Extent3D extent, vk::Format format )
    : engine( engine ), extent( extent ), format( format ), layout( vk::ImageLayout::eUndefined )
  {
    allocateImage();
    createImageView();

    spdlog::info( "Created image" );
  }

  Image::~Image()
  {
    // destroy image view
    engine.getRenderer().getDevice().destroyImageView( imageView );
    spdlog::info( "Destroyed image view" );

    engine.getMemoryAllocator().destroyImage( allocatedImage );
    spdlog::info( "Destroyed image memory" );
  }

  void Image::allocateImage()
  {
    vk::ImageCreateInfo createInfo;
    createInfo.setImageType( vk::ImageType::e2D );
    createInfo.setExtent( extent );
    createInfo.setMipLevels( 1 );
    createInfo.setArrayLayers( 1 );
    createInfo.setFormat( format );
    createInfo.setTiling( vk::ImageTiling::eOptimal );
    createInfo.setInitialLayout( vk::ImageLayout::eUndefined );
    createInfo.setUsage( vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc );
    createInfo.setSamples( vk::SampleCountFlagBits::e4 );
    createInfo.setSharingMode( vk::SharingMode::eExclusive );

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    allocatedImage = engine.getMemoryAllocator().createImage( createInfo, allocInfo );
  }

  void Image::createImageView()
  {
    vk::ImageViewCreateInfo createInfo;
    createInfo.setImage( allocatedImage.image );
    createInfo.setViewType( vk::ImageViewType::e2D );
    createInfo.setFormat( format );
    createInfo.setComponents( { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA } );
    createInfo.setSubresourceRange( { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } );

    imageView = engine.getRenderer().getDevice().createImageView( createInfo );

    spdlog::info( "Created image view" );
  }

  void Image::transitionLayout( const vk::ImageLayout newLayout, const vk::CommandBuffer commandBuffer, const SyncBarriers & barriers )
  {
    // create a barrier to transition the image layout
    // use the pipelineBarrier2KHRs to use synchronization2
    vk::ImageMemoryBarrier2KHR barrier;
    barrier.srcStageMask        = barriers.srcStage;
    barrier.srcAccessMask       = barriers.srcAccess;
    barrier.dstStageMask        = barriers.dstStage;
    barrier.dstAccessMask       = barriers.dstAccess;
    barrier.oldLayout           = layout;
    barrier.newLayout           = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = allocatedImage.image;
    barrier.subresourceRange    = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };

    vk::DependencyInfoKHR dependencyInfo;
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers    = &barrier;

    commandBuffer.pipelineBarrier2KHR( dependencyInfo );

    layout = newLayout;
  }

}  // namespace braque