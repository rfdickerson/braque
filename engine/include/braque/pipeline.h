//
// Created by Robert F. Dickerson on 12/20/24.
//

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan.hpp>

namespace braque {

class Shader;

class Pipeline {
public:
  explicit Pipeline(vk::Device device, Shader& shader, vk::DescriptorSetLayout descriptor_set_layout, bool is_sky_pipeline=false);
  ~Pipeline();

  Pipeline(const Pipeline& other) = delete;
  Pipeline(Pipeline&& other) noexcept = delete;
  Pipeline& operator=(const Pipeline& other) = delete;
  Pipeline& operator=(Pipeline&& other) noexcept = delete;

  void Bind(vk::CommandBuffer buffer);
  static void SetScissor(vk::CommandBuffer buffer, vk::Rect2D);
  static void SetViewport(vk::CommandBuffer buffer, const vk::Viewport & viewport );
  static void Draw(vk::CommandBuffer buffer);

  [[nodiscard]] vk::PipelineLayout VulkanLayout() const;

private:
  void createPipeline(bool is_sky_pipeline);

  vk::Device device;
  vk::PipelineLayout layout_;
  vk::Pipeline pipeline;
  vk::DescriptorSetLayout descriptor_set_layout_;
  Shader& shader_;
};

} // namespace braque

#endif //PIPELINE_HPP
