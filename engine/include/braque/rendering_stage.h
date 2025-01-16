//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef RENDERING_STAGE_HPP
#define RENDERING_STAGE_HPP

#include <memory>
#include <vulkan/vulkan.hpp>

#include "braque/pipeline.h"
#include "braque/asset_loader.h"

namespace braque {
// Forward declarations
class EngineContext;
class Image;
class Shader;
class Uniforms;
class Swapchain;

class RenderingStage {
 public:
  explicit RenderingStage(EngineContext& engine, Swapchain& swapchain, Uniforms& uniforms, AssetLoader& assetLoader);
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

  std::vector<Image>& GetColorImages() { return colorImages; }
  std::vector<Image>& GetDepthImages() { return depthImages; }
  std::vector<Image>& GetPostprocessingImages() { return postprocessingImages; }

  //void renderTriangle(vk::CommandBuffer buffer) const;
  // void render();

 private:
  EngineContext& engine;
  Swapchain & swapchain_;
  AssetLoader & assetLoader_;

  vk::DescriptorPool descriptorPool;

  std::vector<Image> colorImages;
  std::vector<Image> depthImages;

  std::vector<Image> postprocessingImages;

  std::unique_ptr<Shader> shader;
  std::unique_ptr<Pipeline> pipeline;

  std::unique_ptr<Shader> sky_shader_;

  void createDescriptorPool();
};

}  // namespace braque

#endif  // RENDERING_STAGE_HPP
