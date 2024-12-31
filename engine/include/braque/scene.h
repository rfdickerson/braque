//
// Created by Robert F. Dickerson on 12/27/24.
//

#ifndef SCENE_H
#define SCENE_H
#include "memory_allocator.h"

#include <glm/vec3.hpp>
#include <vector>

#include "buffer.h"

namespace braque {
class Engine;
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
  Scene(Engine& engine);
  ~Scene();

  void UploadSceneData();
  void Draw(vk::CommandBuffer buffer);
  void AddCube(glm::vec3 position);

private:

  Engine& engine_;

  Buffer vertex_buffer_;
  Buffer index_buffer_;

  Buffer vertex_staging_buffer_;
  Buffer index_staging_buffer_;

  std::vector<Mesh> meshes_;

};

} // namespace braque

#endif //SCENE_H
