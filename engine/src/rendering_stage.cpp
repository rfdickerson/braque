//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "braque/rendering_stage.h"

#include <vulkan/vulkan.hpp>

#include "braque/image.h"
#include "braque/pipeline.h"
#include "braque/renderer.h"
#include "braque/shader.h"
#include "braque/swapchain.h"
#include "braque/uniforms.h"

#include <spdlog/spdlog.h>
#include <memory>

namespace braque {

RenderingStage::RenderingStage(EngineContext& engine, Swapchain& swapchain, Uniforms& uniforms, AssetLoader& assetLoader) : engine(engine), swapchain_(swapchain), assetLoader_(assetLoader) {
  spdlog::info("Creating rendering stage");

  createDescriptorPool();

  const auto extent = vk::Extent3D{swapchain.getExtent(), 1};

  assetLoader_.openArchive("../../../../test.gaff");

  shader = std::make_unique<Shader>(engine.getRenderer().getDevice(),
                                    "../../../../assets/shaders/triangle.vert.spv", "../../../../assets/shaders/triangle.frag.spv");

  // load sky shader
  sky_shader_ = std::make_unique<Shader>(engine.getRenderer().getDevice(), "../../../../assets/shaders/sky.vert.spv", "../../../../assets/shaders/sky.frag.spv");

  pipeline =
      std::make_unique<Pipeline>(engine.getRenderer().getDevice(), *shader,
                                 uniforms.GetDescriptorSetLayout());

  sky_pipeline_ = std::make_unique<Pipeline>(engine.getRenderer().getDevice(), *sky_shader_, uniforms.GetDescriptorSetLayout(), true);

  colorImages.reserve(Swapchain::getFramesInFlightCount());

  auto colorImageConfig = ImageConfig{};
  colorImageConfig.extent = extent;
  colorImageConfig.format = vk::Format::eR16G16B16A16Sfloat;
  colorImageConfig.usage = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
  colorImageConfig.samples = 4;
  colorImageConfig.mipLevels = 1;

  auto depthImageConfig = ImageConfig{};
  depthImageConfig.extent = extent;
  depthImageConfig.format = vk::Format::eD32Sfloat;
  depthImageConfig.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
  depthImageConfig.samples = 4;
  depthImageConfig.mipLevels = 1;

  auto postprocessingImageConfig = ImageConfig{};
  postprocessingImageConfig.extent = extent;
  postprocessingImageConfig.format = vk::Format::eR16G16B16A16Sfloat;
  postprocessingImageConfig.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eColorAttachment;
  postprocessingImageConfig.samples = 1;
  postprocessingImageConfig.mipLevels = 1;
  
  for (uint32_t i = 0; i < Swapchain::getFramesInFlightCount();
       ++i) {

    colorImages.emplace_back(engine, colorImageConfig);
    depthImages.emplace_back(engine, depthImageConfig);
    postprocessingImages.emplace_back(engine, postprocessingImageConfig);
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
  // switch to color attachment
  renderingAttachmentInfo.setImageView(colorImages[curr].GetImageView());

  // create the depth attachment
  auto clear_value = vk::ClearValue();
  clear_value.setDepthStencil({0.0F, 0}); 

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


}  // namespace braque
