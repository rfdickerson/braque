//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef RENDERING_STAGE_HPP
#define RENDERING_STAGE_HPP

#include <memory>
#include <vulkan/vulkan.hpp>

#include "braque/pipeline.h"

namespace braque {
// Forward declarations
class Engine;
class Image;
class Shader;

class RenderingStage {
 public:
  explicit RenderingStage(Engine& engine);
  ~RenderingStage();

  // make sure copy and move are deleted
  RenderingStage(const RenderingStage&) = delete;
  auto operator=(const RenderingStage&) -> RenderingStage& = delete;
  RenderingStage(RenderingStage&&) = delete;
  auto operator=(RenderingStage&&) -> RenderingStage& = delete;

  [[nodiscard]] auto getDescriptorPool() const -> vk::DescriptorPool {
    return descriptorPool;
  }

  // get PipelineLayout
  [[nodiscard]] auto GetPipeline() const -> Pipeline& { return *pipeline; }

  static void begin(vk::CommandBuffer buffer);
  void beginRenderingPass(vk::CommandBuffer buffer) const;
  void prepareImageForColorAttachment(vk::CommandBuffer buffer) const;
  void prepareImageForDisplay(vk::CommandBuffer buffer) const;
  static void endRenderingPass(vk::CommandBuffer buffer);
  static void end(vk::CommandBuffer buffer);

  void renderTriangle(vk::CommandBuffer buffer) const;
  // void render();

 private:
  Engine& engine;

  vk::DescriptorPool descriptorPool;
  std::unique_ptr<Image> offscreenImage;
  std::unique_ptr<Shader> shader;
  std::unique_ptr<Pipeline> pipeline;

  void createDescriptorPool();
};

}  // namespace braque

#endif  // RENDERING_STAGE_HPP
