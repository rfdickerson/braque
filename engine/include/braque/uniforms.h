#pragma once

#include "braque/memory_allocator.h"

#include <glm/glm.hpp>
#include <vector>

namespace braque {

class Engine;
class Texture;

class Uniforms {
 public:
  Uniforms(Engine& engine);
  ~Uniforms();

  // remove copy and move
  Uniforms(const Uniforms&) = delete;

  void SetCameraData(vk::CommandBuffer buffer, const Camera& camera);

  void SetTextureData(const Texture& texture, vk::Sampler sampler);

 // bind descriptor sets
  void Bind(vk::CommandBuffer buffer) const;

  auto GetDescriptorSetLayout() const -> vk::DescriptorSetLayout { return descriptor_set_layout_; }

 private:
  Engine& engine_;

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