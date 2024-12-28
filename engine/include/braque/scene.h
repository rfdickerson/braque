//
// Created by Robert F. Dickerson on 12/27/24.
//

#ifndef SCENE_H
#define SCENE_H
#include "memory_allocator.h"

#include <glm/vec3.hpp>

namespace braque {

constexpr vk::DeviceSize kVertexBufferSize = 32000;

class Scene {
public:
  Scene(MemoryAllocator &allocator, Renderer& renderer);
  ~Scene();

  void UploadSceneData();
  void Draw(vk::CommandBuffer buffer);
  void AddCube(glm::vec3 position);

private:

  MemoryAllocator& allocator_;
  Renderer& renderer_;

  AllocatedBuffer vertex_buffer_;
  AllocatedBuffer index_buffer_;

  AllocatedBuffer staging_buffer_;

  void CreateVertexBuffer();
  void CreateIndexBuffer();
  void CreateStagingBuffer();
};

} // namespace braque

#endif //SCENE_H
