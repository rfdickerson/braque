//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/swapchain.h"

#include <spdlog/spdlog.h>

namespace braque
{
  Swapchain::Swapchain( Window & window, EngineContext & context ) : context_( context ), swapchainFormat( vk::Format::eUndefined )
  {
    createSwapchain( window );
    createSemaphores();
    createFences();
    createSwapchainImages();
    createCommandBuffers();

    spdlog::info( "Created the swapchain" );
  }

  Swapchain::~Swapchain()
  {
    // wait for device to be idle
    context_.getRenderer().getDevice().waitIdle();

    // delete the command pool
    context_.getRenderer().getDevice().destroyCommandPool( commandPool );

    // delete the semaphores
    for ( auto semaphore : imageAvailableSemaphores )
    {
      context_.getRenderer().getDevice().destroySemaphore( semaphore );
    }

    for ( auto semaphore : renderFinishedSemaphores )
    {
      context_.getRenderer().getDevice().destroySemaphore( semaphore );
    }

    // delete the fences
    for ( auto fence : inFlightFences )
    {
      context_.getRenderer().getDevice().destroyFence( fence );
    }

    context_.getRenderer().getDevice().destroySwapchainKHR( swapchain_ );

    // delete the surface
    context_.getRenderer().getInstance().destroySurfaceKHR( surface_ );

    spdlog::info( "Destroyed the swapchain" );
  }

  void Swapchain::waitForFrame() const
  {
    const auto fence  = inFlightFences[currentFrameInFlight];
    auto       result = context_.getRenderer().getDevice().waitForFences( 1, &fence, VK_TRUE, UINT64_MAX );

    if ( result != vk::Result::eSuccess )
    {
      spdlog::error( "Failed to wait for fence" );
    }
  }

  void Swapchain::waitForImageInFlight()
  {
    auto imageFence    = imagesInFlight[currentImageIndex];
    auto inFlightFence = inFlightFences[currentFrameInFlight];

    if ( imageFence != VK_NULL_HANDLE )
    {
      auto result = context_.getRenderer().getDevice().waitForFences( 1, &imageFence, VK_TRUE, UINT64_MAX );
      if ( result != vk::Result::eSuccess )
      {
        spdlog::error( "Failed to wait for image in flight fence" );
      }
    }

    imagesInFlight[currentImageIndex] = inFlightFences[currentFrameInFlight];

    // reset the fence
    auto resetResult = context_.getRenderer().getDevice().resetFences( 1, &inFlightFence );

    if ( resetResult != vk::Result::eSuccess )
    {
      spdlog::error( "Failed to reset fence" );
    }

    frameStats.Update();
  }

  void Swapchain::acquireNextImage()
  {
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{};
    acquireNextImageInfo.setSwapchain( swapchain_ );
    acquireNextImageInfo.setTimeout( UINT64_MAX );
    acquireNextImageInfo.setSemaphore( imageAvailableSemaphores[currentFrameInFlight] );
    acquireNextImageInfo.setDeviceMask( 1 );
    ;

    auto result = context_.getRenderer().getDevice().acquireNextImage2KHR( acquireNextImageInfo );

    currentImageIndex = result.value;
  }

  void Swapchain::presentImage()
  {
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setSwapchainCount( 1 );
    presentInfo.setPSwapchains( &swapchain_ );
    presentInfo.setPImageIndices( &currentImageIndex );
    presentInfo.setWaitSemaphoreCount( 1 );
    presentInfo.setPWaitSemaphores( &renderFinishedSemaphores[currentFrameInFlight] );

    auto result = context_.getRenderer().getGraphicsQueue().presentKHR( presentInfo );

    if ( result != vk::Result::eSuccess )
    {
      spdlog::error( "Failed to present image" );
    }

    currentFrameInFlight = ( currentFrameInFlight + 1 ) % MAX_FRAMES_IN_FLIGHT;
  }

  void Swapchain::createSwapchain( const Window & window )
  {
    surface_ = window.CreateSurface( context_.getRenderer() );

    // get the surface capabilities
    auto surfaceCapabilities = context_.getRenderer().getPhysicalDevice().getSurfaceCapabilitiesKHR( surface_ );
    auto surfaceFormats      = context_.getRenderer().getPhysicalDevice().getSurfaceFormatsKHR( surface_ );
    auto presentModes        = context_.getRenderer().getPhysicalDevice().getSurfacePresentModesKHR( surface_ );

    vk::SurfaceFormatKHR surfaceFormat;

    // find the surface format with SRGB
    for ( auto format : surfaceFormats )
    {
      if ( format.format == vk::Format::eB8G8R8A8Srgb )
      {
        surfaceFormat = format;
        break;
      }
    }

    // check for FIFO present mode
    auto presentMode = vk::PresentModeKHR::eFifo;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.setSurface( surface_ );
    swapchainCreateInfo.setMinImageCount( surfaceCapabilities.minImageCount + 1 );
    swapchainCreateInfo.setImageFormat( surfaceFormat.format );
    swapchainCreateInfo.setImageColorSpace( surfaceFormat.colorSpace );
    swapchainCreateInfo.setImageExtent( surfaceCapabilities.currentExtent );
    swapchainCreateInfo.setImageArrayLayers( 1 );
    swapchainCreateInfo.setImageUsage( vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst );
    swapchainCreateInfo.setImageSharingMode( vk::SharingMode::eExclusive );
    swapchainCreateInfo.setQueueFamilyIndexCount( 0 );
    swapchainCreateInfo.setPQueueFamilyIndices( nullptr );
    swapchainCreateInfo.setPresentMode( presentMode );

    auto result = context_.getRenderer().getDevice().createSwapchainKHR( swapchainCreateInfo );

    swapchain_ = result;

    // get the image count
    imageCount = context_.getRenderer().getDevice().getSwapchainImagesKHR( swapchain_ ).size();

    // set the extent
    swapchainExtent = surfaceCapabilities.currentExtent;
    swapchainFormat = surfaceFormat.format;
  }

  void Swapchain::createSemaphores()
  {
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};

    // semaphores per swapchain image
    for ( uint32_t i = 0; i < imageCount; i++ )
    {
      imageAvailableSemaphores.push_back( context_.getRenderer().getDevice().createSemaphore( semaphoreCreateInfo ) );
      renderFinishedSemaphores.push_back( context_.getRenderer().getDevice().createSemaphore( semaphoreCreateInfo ) );
    }
  }

  void Swapchain::createFences()
  {
    // create fences for each frame in flight
    vk::FenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.setFlags( vk::FenceCreateFlagBits::eSignaled );

    for ( int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
    {
      const auto fence = context_.getRenderer().getDevice().createFence( fenceCreateInfo );
      inFlightFences.push_back( fence );
    }

    imagesInFlight.resize( imageCount, VK_NULL_HANDLE );
  }

  void Swapchain::createSwapchainImages()
  {
    auto vkImages = context_.getRenderer().getDevice().getSwapchainImagesKHR( swapchain_ );
    swapchainImages.reserve(vkImages.size());

    for ( const auto& vkImage : vkImages ) {
      swapchainImages.emplace_back(context_, vkImage, swapchainFormat, vk::ImageLayout::eUndefined);
    }

    imageCount = static_cast<uint32_t>(swapchainImages.size());

  }

  void Swapchain::createCommandBuffers()
  {
    // create the command pool
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.setQueueFamilyIndex( context_.getRenderer().getGraphicsQueueFamilyIndex() );
    commandPoolCreateInfo.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );

    commandPool = context_.getRenderer().getDevice().createCommandPool( commandPoolCreateInfo );

    if ( !commandPool )
    {
      spdlog::error( "Failed to create command pool" );
    }

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.setCommandPool( commandPool );
    commandBufferAllocateInfo.setLevel( vk::CommandBufferLevel::ePrimary );
    commandBufferAllocateInfo.setCommandBufferCount( imageCount );

    commandBuffers = context_.getRenderer().getDevice().allocateCommandBuffers( commandBufferAllocateInfo );
  }

  void Swapchain::submitCommandBuffer()
  {
    // get current wait and signal semaphores
    auto wait   = imageAvailableSemaphores[currentFrameInFlight];
    auto signal = renderFinishedSemaphores[currentFrameInFlight];
    auto fence  = inFlightFences[currentFrameInFlight];

    auto commandBuffer = commandBuffers[currentImageIndex];

    vk::SemaphoreSubmitInfo waitSemaphoreInfo{};
    waitSemaphoreInfo.setSemaphore( wait );
    waitSemaphoreInfo.setStageMask( vk::PipelineStageFlagBits2::eColorAttachmentOutput | vk::PipelineStageFlagBits2::eBottomOfPipe );

    vk::SemaphoreSubmitInfo signalSemaphoreInfo{};
    signalSemaphoreInfo.setSemaphore( signal );
    signalSemaphoreInfo.setStageMask( vk::PipelineStageFlagBits2::eColorAttachmentOutput );

    vk::CommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.setCommandBuffer( commandBuffer );

    vk::SubmitInfo2 submitInfo{};
    submitInfo.setWaitSemaphoreInfos( waitSemaphoreInfo );
    submitInfo.setCommandBufferInfos( commandBufferSubmitInfo );
    submitInfo.setSignalSemaphoreInfos( signalSemaphoreInfo );

    context_.getRenderer().getGraphicsQueue().submit2KHR( submitInfo, fence );
  }

}  // namespace braque
