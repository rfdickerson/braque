#pragma once

#include <vector>

#include "braque/engine_context.h"
#include "braque/swapchain.h"
#include "braque/camera.h"
#include "braque/texture.h"
#include "braque/buffer.h"

namespace braque {

class Uniforms {
 public:
  Uniforms(EngineContext& engine, Swapchain& swapchain);
  ~Uniforms();

  // remove copy and move
  Uniforms(const Uniforms&) = delete;

  void SetCameraData(vk::CommandBuffer buffer, const Camera& camera);

  void SetTextureData(const Texture& texture, vk::Sampler sampler);

 // bind descriptor sets
  void Bind(vk::CommandBuffer buffer, vk::PipelineLayout layout) const;

  auto GetDescriptorSetLayout() const -> vk::DescriptorSetLayout { return descriptor_set_layout_; }

 private:
  EngineContext& engine_;
  Swapchain& swapchain_;

  std::vector<Buffer> camera_buffers_;
  std::vector<vk::DescriptorSet> descriptor_sets_;

  vk::DescriptorSetLayout descriptor_set_layout_;
  vk::DescriptorPool descriptor_pool_;

  void CreateUniformBuffers();
  void createDescriptorSetLayout();
  void createDescriptorPool();
  void createDescriptorSets();
};

}  // namespace braque