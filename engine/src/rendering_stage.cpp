//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "braque/rendering_stage.h"

#include "braque/image.h"
#include "braque/pipeline.h"
#include "braque/renderer.h"
#include "braque/shader.h"
#include "braque/swapchain.h"
#include "braque/uniforms.h"

#include <spdlog/spdlog.h>

namespace braque {

RenderingStage::RenderingStage(EngineContext& engine, Swapchain& swapchain, Uniforms& uniforms) : engine(engine), swapchain_(swapchain) {
  spdlog::info("Creating rendering stage");

  createDescriptorPool();

  const auto extent = vk::Extent3D{swapchain.getExtent(), 1};

  offscreenImage =
      std::make_unique<Image>(engine, extent, vk::Format::eR16G16B16A16Sfloat);
  shader = std::make_unique<Shader>(engine.getRenderer().getDevice(),
                                    "../assets/shaders/triangle.vert.spv",
                                    "../assets/shaders/triangle.frag.spv");
  pipeline =
      std::make_unique<Pipeline>(engine.getRenderer().getDevice(), *shader,
                                 uniforms.GetDescriptorSetLayout());

  for (uint32_t i = 0; i < Swapchain::getFramesInFlightCount();
       ++i) {
    depthImages.emplace_back(engine, extent, vk::Format::eD32Sfloat);
  }
}

RenderingStage::~RenderingStage() {
  // destroy the descriptor pool
  engine.getRenderer().getDevice().destroyDescriptorPool(descriptorPool);

  spdlog::info("Destroying rendering stage");
}

void RenderingStage::begin(const vk::CommandBuffer buffer) {
  buffer.begin(vk::CommandBufferBeginInfo{
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
}

void RenderingStage::beginRenderingPass(const vk::CommandBuffer buffer) const {

  const auto curr = swapchain_.CurrentFrameIndex();

  constexpr vk::ClearColorValue clearColor{0.0F, 0.0F, 0.0F, 0.0F};

  vk::RenderingAttachmentInfo renderingAttachmentInfo{};
  renderingAttachmentInfo.setClearValue(clearColor);
  renderingAttachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
  renderingAttachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
  renderingAttachmentInfo.setImageLayout(
      vk::ImageLayout::eColorAttachmentOptimal);
  renderingAttachmentInfo.setImageView(swapchain_.getImageView());

  // create the depth attachment
  auto clear_value = vk::ClearValue();
  clear_value.setDepthStencil({1.0F, 0});

  vk::RenderingAttachmentInfo depthAttachmentInfo{};
  depthAttachmentInfo.setImageView(depthImages[curr].GetImageView());
  depthAttachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
  depthAttachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
  depthAttachmentInfo.setImageLayout(vk::ImageLayout::eDepthAttachmentOptimal);
  depthAttachmentInfo.setClearValue(clear_value);

  const auto renderArea = vk::Rect2D{{0, 0}, swapchain_.getExtent()};

  vk::RenderingInfo renderingInfo{};
  renderingInfo.setColorAttachments(renderingAttachmentInfo);
  renderingInfo.setPDepthAttachment(&depthAttachmentInfo);
  renderingInfo.setLayerCount(1);
  renderingInfo.setRenderArea(renderArea);

  buffer.beginRenderingKHR(renderingInfo);
}

void RenderingStage::endRenderingPass(const vk::CommandBuffer buffer) {
  buffer.endRenderingKHR();
}

void RenderingStage::end(const vk::CommandBuffer buffer) {
  buffer.end();
}

void RenderingStage::prepareImageForColorAttachment(
    const vk::CommandBuffer buffer) const {
  // Define the image memory barrier using synchronization2
  const vk::ImageMemoryBarrier2 imageBarrier{
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStageMask
      vk::AccessFlagBits2::eColorAttachmentRead,           // srcAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // dstStageMask
      vk::AccessFlagBits2::eColorAttachmentWrite,          // dstAccessMask
      vk::ImageLayout::eUndefined,                         // oldLayout
      vk::ImageLayout::eColorAttachmentOptimal,            // newLayout
      vk::QueueFamilyIgnored,                        // srcQueueFamilyIndex
      vk::QueueFamilyIgnored,                        // dstQueueFamilyIndex
      swapchain_.swapchain_image(),       // image
      {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}  // subresourceRange
  };

  // Encapsulate the barrier in a dependency info object
  vk::DependencyInfo dependencyInfo{};
  dependencyInfo.setImageMemoryBarriers(imageBarrier);

  // Issue the pipeline barrier with synchronization2
  buffer.pipelineBarrier2KHR(dependencyInfo);
}

void RenderingStage::prepareImageForDisplay(
    const vk::CommandBuffer buffer) const {
  // Define the image memory barrier using synchronization2
  const vk::ImageMemoryBarrier2 imageBarrier{
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // srcStageMask
      vk::AccessFlagBits2::eColorAttachmentWrite,          // srcAccessMask
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,  // dstStageMask
      vk::AccessFlagBits2::eNone,                          // dstAccessMask
      vk::ImageLayout::eColorAttachmentOptimal,            // oldLayout
      vk::ImageLayout::ePresentSrcKHR,                     // newLayout
      vk::QueueFamilyIgnored,                        // srcQueueFamilyIndex
      vk::QueueFamilyIgnored,                        // dstQueueFamilyIndex
      swapchain_.swapchain_image(),       // image
      {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}  // subresourceRange
  };

  // Encapsulate the barrier in a dependency info object
  vk::DependencyInfo dependencyInfo{};
  dependencyInfo.setImageMemoryBarriers(imageBarrier);

  // Issue the pipeline barrier with synchronization2
  buffer.pipelineBarrier2KHR(dependencyInfo);
}

void RenderingStage::createDescriptorPool() {
  constexpr auto descriptorCount = 1000;

  constexpr std::array poolSizes = {
      vk::DescriptorPoolSize{vk::DescriptorType::eSampler, descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic,
                             descriptorCount},
      vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment,
                             descriptorCount}};

  const vk::DescriptorPoolCreateInfo poolInfo =
      vk::DescriptorPoolCreateInfo{}
          .setPoolSizes(poolSizes)
          .setMaxSets(descriptorCount)
          .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

  descriptorPool =
      engine.getRenderer().getDevice().createDescriptorPool(poolInfo, nullptr);
}

void RenderingStage::renderTriangle(const vk::CommandBuffer buffer) const {
  pipeline->Bind(buffer);

  const auto viewport =
      vk::Viewport{0.0F,
                   0.0F,
                   static_cast<float>(swapchain_.getExtent().width),
                   static_cast<float>(swapchain_.getExtent().height),
                   0.0F,
                   1.0F};
  Pipeline::SetViewport(buffer, viewport);

  const auto scissor = vk::Rect2D{{0, 0}, swapchain_.getExtent()};
  Pipeline::SetScissor(buffer, scissor);

  buffer.draw(3, 1, 0, 0);
}

}  // namespace braque
