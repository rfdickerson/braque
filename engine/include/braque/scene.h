//
// Created by Robert F. Dickerson on 12/27/24.
//

#ifndef SCENE_H
#define SCENE_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "buffer.h"
#include "asset_loader.h"

namespace braque {

// forward declarations
class EngineContext;
class Texture;
class Uniforms;
class AssetLoader;

constexpr vk::DeviceSize kVertexBufferSize = 500000;

struct Mesh {
  std::string name;
  int32_t vertex_offset;
  uint32_t index_offset;
  uint32_t index_count;
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 uv;
};

class Scene {
public:
  Scene(EngineContext& engine, Uniforms& uniforms, AssetLoader& assetLoader);
  ~Scene();

  void UploadSceneData();
  void Draw(vk::CommandBuffer buffer);
  void AddCube();
  void AddTerrain();

private:

  EngineContext& engine_;
  AssetLoader& assetLoader_;

  Buffer vertex_buffer_;
  Buffer index_buffer_;

  Buffer vertex_staging_buffer_;
  Buffer index_staging_buffer_;

  std::vector<Mesh> meshes_;
  Texture* texture_;

  vk::Sampler texture_sampler_;

  void CreateTextureSampler();

};

} // namespace braque

#endif //SCENE_H
