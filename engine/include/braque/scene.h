//
// Created by Robert F. Dickerson on 12/27/24.
//

#ifndef SCENE_H
#define SCENE_H
#include "memory_allocator.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include <memory>

#include "braque/buffer.h"


namespace braque {

class Engine; // Forward declaration if needed
class Texture;

constexpr vk::DeviceSize kVertexBufferSize = 32000;

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
  explicit Scene(Engine& engine);
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
  Texture* texture_;

};

} // namespace braque

#endif //SCENE_H
