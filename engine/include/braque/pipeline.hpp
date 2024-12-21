//
// Created by Robert F. Dickerson on 12/20/24.
//

#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <vulkan/vulkan.hpp>

#include <braque/shader.hpp>

namespace braque {

class Pipeline {
public:
  explicit Pipeline(vk::Device device, Shader& shader);
  ~Pipeline();

  void Bind(vk::CommandBuffer buffer);
  static void SetScissor(vk::CommandBuffer buffer, vk::Rect2D);
  static void SetViewport(vk::CommandBuffer buffer, const vk::Viewport & viewport );
  static void Draw(vk::CommandBuffer buffer);

private:
  vk::Device device;
  vk::PipelineLayout layout;
  vk::Pipeline pipeline;
};

} // braque

#endif //PIPELINE_HPP
