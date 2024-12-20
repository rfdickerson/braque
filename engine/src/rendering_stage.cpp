//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "braque/rendering_stage.hpp"

#include "braque/image.hpp"
#include "braque/renderer.hpp"
#include "braque/shader.hpp"
#include "braque/swapchain.hpp"

#include <spdlog/spdlog.h>

namespace braque
{

  RenderingStage::RenderingStage( Engine & engine ) : engine( engine )
  {
    spdlog::info( "Creating rendering stage" );

    createDescriptorPool();

    const auto extent = vk::Extent3D{ engine.getSwapchain().getExtent(), 1 };

    offscreenImage = std::make_unique<Image>( engine, extent, vk::Format::eR16G16B16A16Sfloat );
    shader         = std::make_unique<Shader>( engine.getRenderer().getDevice(), "assets/shaders/triangle.vert.spv", "assets/shaders/triangle.frag.spv" );
  }

  RenderingStage::~RenderingStage()
  {
    // destroy the descriptor pool
    engine.getRenderer().getDevice().destroyDescriptorPool( descriptorPool );

    spdlog::info( "Destroying rendering stage" );
  }

  void RenderingStage::begin( const vk::CommandBuffer buffer )
  {
    buffer.begin( vk::CommandBufferBeginInfo{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit } );
  }

  void RenderingStage::beginRenderingPass( const vk::CommandBuffer buffer ) const
  {
    const auto & swapchain = engine.getSwapchain();

    constexpr vk::ClearColorValue clearColor{ 0.0F, 0.0F, 0.0F, 0.0F };

    vk::RenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.setClearValue( clearColor );
    renderingAttachmentInfo.setLoadOp( vk::AttachmentLoadOp::eClear );
    renderingAttachmentInfo.setStoreOp( vk::AttachmentStoreOp::eStore );
    renderingAttachmentInfo.setImageLayout( vk::ImageLayout::eColorAttachmentOptimal );
    renderingAttachmentInfo.setImageView( swapchain.getImageView() );

    const auto renderArea = vk::Rect2D{ { 0, 0 }, swapchain.getExtent() };

    vk::RenderingInfo renderingInfo{};
    renderingInfo.setColorAttachments( renderingAttachmentInfo );
    renderingInfo.setLayerCount( 1 );
    renderingInfo.setRenderArea( renderArea );

    buffer.beginRenderingKHR( renderingInfo );
  }

  void RenderingStage::endRenderingPass( const vk::CommandBuffer buffer )
  {
    buffer.endRenderingKHR();
  }

  void RenderingStage::end( const vk::CommandBuffer buffer )
  {
    buffer.end();
  }

  void RenderingStage::prepareImageForColorAttachment( const vk::CommandBuffer buffer ) const
  {
    // Define the image memory barrier using synchronization2
    const vk::ImageMemoryBarrier2 imageBarrier{
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStageMask
      vk::AccessFlagBits2::eColorAttachmentRead,           // srcAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // dstStageMask
      vk::AccessFlagBits2::eColorAttachmentWrite,          // dstAccessMask
      vk::ImageLayout::eUndefined,                         // oldLayout
      vk::ImageLayout::eColorAttachmentOptimal,            // newLayout
      vk::QueueFamilyIgnored,                              // srcQueueFamilyIndex
      vk::QueueFamilyIgnored,                              // dstQueueFamilyIndex
      engine.getSwapchain().getSwapchainImage(),           // image
      { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }      // subresourceRange
    };

    // Encapsulate the barrier in a dependency info object
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setImageMemoryBarriers( imageBarrier );

    // Issue the pipeline barrier with synchronization2
    buffer.pipelineBarrier2KHR( dependencyInfo );
  }

  void RenderingStage::prepareImageForDisplay( const vk::CommandBuffer buffer ) const
  {
    // Define the image memory barrier using synchronization2
    const vk::ImageMemoryBarrier2 imageBarrier{
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStageMask
      vk::AccessFlagBits2::eColorAttachmentWrite,          // srcAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // dstStageMask
      vk::AccessFlagBits2::eNone,                          // dstAccessMask
      vk::ImageLayout::eColorAttachmentOptimal,            // oldLayout
      vk::ImageLayout::ePresentSrcKHR,                     // newLayout
      vk::QueueFamilyIgnored,                              // srcQueueFamilyIndex
      vk::QueueFamilyIgnored,                              // dstQueueFamilyIndex
      engine.getSwapchain().getSwapchainImage(),           // image
      { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }      // subresourceRange
    };

    // Encapsulate the barrier in a dependency info object
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setImageMemoryBarriers( imageBarrier );

    // Issue the pipeline barrier with synchronization2
    buffer.pipelineBarrier2KHR( dependencyInfo );
  }

  void RenderingStage::createDescriptorPool()
  {
    constexpr auto descriptorCount = 1000;

    constexpr std::array poolSizes = { vk::DescriptorPoolSize{ vk::DescriptorType::eSampler, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, descriptorCount },
                                       vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment, descriptorCount } };

    const vk::DescriptorPoolCreateInfo poolInfo =
      vk::DescriptorPoolCreateInfo{}.setPoolSizes( poolSizes ).setMaxSets( descriptorCount ).setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet );

    descriptorPool = engine.getRenderer().getDevice().createDescriptorPool( poolInfo, nullptr );
  }

}  // namespace braque
