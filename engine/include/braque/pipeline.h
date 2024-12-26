//
// Created by Robert F. Dickerson on 12/20/24.
//

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan.hpp>

#include <braque/shader.h>

namespace braque {

class Pipeline {
public:
  explicit Pipeline(vk::Device device, Shader& shader, vk::DescriptorSetLayout descriptor_set_layout);
  ~Pipeline();

  // make sure copy and move are deleted
    Pipeline(const Pipeline&) = delete;
  Pipeline& operator=(const Pipeline&) = delete;

  void Bind(vk::CommandBuffer buffer);
  static void SetScissor(vk::CommandBuffer buffer, vk::Rect2D);
  static void SetViewport(vk::CommandBuffer buffer, const vk::Viewport & viewport );
  static void Draw(vk::CommandBuffer buffer);

    vk::PipelineLayout VulkanLayout() const;

private:
  vk::Device device;
  vk::PipelineLayout layout_;
  vk::Pipeline pipeline;
};

} // braque

#endif //PIPELINE_HPP
