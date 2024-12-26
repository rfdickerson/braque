//
// Created by rfdic on 12/17/2024.
//

#include "braque/image.h"

#include "braque/renderer.h"

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

  void Image::blitImage( const vk::CommandBuffer buffer, const Image & destImage ) const
  {
    // check that source is in a transfer source layout
    if ( layout != vk::ImageLayout::eTransferSrcOptimal )
    {
      spdlog::error( "Source image is not in transfer source optimal layout" );
      throw std::runtime_error( "Source image is not in transfer source optimal layout" );
    }

    // check that destination is in a transfer destination layout
    if ( destImage.getLayout() != vk::ImageLayout::eTransferDstOptimal )
    {
      spdlog::error( "Destination image is not in transfer destination optimal layout" );
      throw std::runtime_error( "Destination image is not in transfer destination optimal layout" );
    }

    vk::ImageBlit2KHR regions;
    regions.srcOffsets[0]  = vk::Offset3D{ 0, 0, 0 };
    regions.dstOffsets[0]  = vk::Offset3D{ 0, 0, 0 };
    regions.srcOffsets[1]  = vk::Offset3D{ static_cast<int32_t>( extent.width ), static_cast<int32_t>( extent.height ), 1 };
    regions.dstOffsets[1]  = vk::Offset3D{ static_cast<int32_t>( destImage.getExtent().width ), static_cast<int32_t>( destImage.getExtent().height ), 1 };
    regions.srcSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 };

    // do the blit
    vk::BlitImageInfo2KHR blitInfo;
    blitInfo.setSrcImage( allocatedImage.image );
    blitInfo.setSrcImageLayout( layout );
    blitInfo.setDstImage( destImage.allocatedImage.image );
    blitInfo.setRegions( regions );
    blitInfo.setFilter( vk::Filter::eLinear );

    buffer.blitImage2KHR( blitInfo );
  }

}  // namespace braque