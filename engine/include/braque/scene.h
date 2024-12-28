//
// Created by Robert F. Dickerson on 12/27/24.
//

#ifndef SCENE_H
#define SCENE_H
#include "memory_allocator.h"

#include <glm/vec3.hpp>
#include <vector>

namespace braque {

constexpr vk::DeviceSize kVertexBufferSize = 32000;

struct Mesh {
  std::string name;
  uint32_t vertex_offset;
  uint32_t index_offset;
  uint32_t index_count;
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

class Scene {
public:
  Scene(MemoryAllocator &allocator, Renderer& renderer);
  ~Scene();

  void UploadSceneData(vk::CommandBuffer buffer);
  void Draw(vk::CommandBuffer buffer);
  void AddCube(glm::vec3 position);

private:

  MemoryAllocator& allocator_;
  Renderer& renderer_;

  AllocatedBuffer vertex_buffer_;
  AllocatedBuffer index_buffer_;
  AllocatedBuffer vertex_staging_buffer_;
  AllocatedBuffer index_staging_buffer_;

  std::vector<Mesh> meshes_;

  void CreateVertexBuffer();
  void CreateIndexBuffer();
  void CreateStagingBuffer();
};

} // namespace braque

#endif //SCENE_H
