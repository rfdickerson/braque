//
// Created by Robert F. Dickerson on 12/27/24.
//

#include "braque/scene.h"

#include "braque/engine.h"

namespace braque {
Scene::Scene(Engine& engine)
    : engine_(engine),
      vertex_buffer_(engine, BufferType::vertex, kVertexBufferSize),
      index_buffer_(engine, BufferType::index, kVertexBufferSize),
      vertex_staging_buffer_(engine, BufferType::staging, kVertexBufferSize),
      index_staging_buffer_(engine, BufferType::staging, kVertexBufferSize) {

  // add a cube to vertex and index staging buffers
  AddCube({0.0f, 0.0f, 0.0f});
  UploadSceneData();
}

Scene::~Scene() {}

void Scene::Draw(vk::CommandBuffer buffer) {

  vertex_buffer_.Bind(buffer);
  index_buffer_.Bind(buffer);

  for (const auto& mesh : meshes_) {
    // draw the mesh
    buffer.drawIndexed(mesh.index_count, 1, mesh.index_offset,
                       mesh.vertex_offset, 0);
  }
}

void Scene::UploadSceneData() {

  const auto device = engine_.getRenderer().getDevice();
  const auto graphicsQueue = engine_.getRenderer().getGraphicsQueue();

  const auto command_buffer = engine_.getRenderer().CreateCommandBuffer();

  // begin single time command buffer
  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  command_buffer.begin(beginInfo);

  vertex_staging_buffer_.CopyToBuffer(command_buffer, vertex_buffer_);
  index_staging_buffer_.CopyToBuffer(command_buffer, index_buffer_);

  // end the command buffer
  command_buffer.end();

  // submit
  vk::SubmitInfo submitInfo;
  submitInfo.setCommandBuffers(command_buffer);
  graphicsQueue.submit(submitInfo, nullptr);
}

void Scene::AddCube(glm::vec3 position) {

  // Face 1..6 each has 6 vertices = 36 total
// Indices will then be 6 faces × 2 triangles × 3 indices = 36 indices
// Face ordering: Front (Blue), Back (Red), Left (Green), Right (Yellow), Bottom (Cyan), Top (Magenta).

std::vector<Vertex> vertices = {
    // Face 1 (Front, Blue), z = -0.5
    // bottom-left, bottom-right, top-right, top-left, repeat bottom-left, repeat top-right
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [0]
    {{ 0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [1]
    {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [2]
    {{-0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [3]
    {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [4] duplicate of [0]
    {{ 0.5f,  0.5f, -0.5f}, {0, 0, -1}, {0, 0, 1}}, // [5] duplicate of [2]

    // Face 2 (Back, Red), z = +0.5
    {{-0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [6]  bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [7]  bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [8]  top-right
    {{-0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [9]  top-left
    {{-0.5f, -0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [10] duplicate of [6]
    {{ 0.5f,  0.5f,  0.5f}, {0, 0,  1}, {1, 0, 0}}, // [11] duplicate of [8]

    // Face 3 (Left, Green), x = -0.5
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [12] bottom-left
    {{-0.5f, -0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [13] bottom-right
    {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [14] top-right
    {{-0.5f,  0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [15] top-left
    {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [16] duplicate of [12]
    {{-0.5f,  0.5f,  0.5f}, {-1, 0, 0}, {0, 1, 0}}, // [17] duplicate of [14]

    // Face 4 (Right, Yellow), x = +0.5
    {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}}, // [18] bottom-left
    {{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}}, // [19] bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}}, // [20] top-right
    {{ 0.5f,  0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}}, // [21] top-left
    {{ 0.5f, -0.5f, -0.5f}, {1, 0, 0}, {1, 1, 0}}, // [22] duplicate of [18]
    {{ 0.5f,  0.5f,  0.5f}, {1, 0, 0}, {1, 1, 0}}, // [23] duplicate of [20]

    // Face 5 (Bottom, Cyan), y = -0.5
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}}, // [24] bottom-left
    {{ 0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}}, // [25] bottom-right
    {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}}, // [26] top-right
    {{-0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}}, // [27] top-left
    {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 1, 1}}, // [28] duplicate of [24]
    {{ 0.5f, -0.5f,  0.5f}, {0, -1, 0}, {0, 1, 1}}, // [29] duplicate of [26]

    // Face 6 (Top, Magenta), y = +0.5
    {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}}, // [30] bottom-left
    {{ 0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}}, // [31] bottom-right
    {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}}, // [32] top-right
    {{-0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}}, // [33] top-left
    {{-0.5f,  0.5f, -0.5f}, {0, 1, 0}, {1, 0, 1}}, // [34] duplicate of [30]
    {{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0, 1}}, // [35] duplicate of [32]
};


  std::vector<uint32_t> indices = {
    // Front:   uses vertices [0..5]
    0, 1, 2,   2, 3, 0,

    // Back:    uses vertices [6..11]
    6, 7, 8,   8, 9, 6,

    // Left:    uses vertices [12..17]
    12, 13, 14, 14, 15, 12,

    // Right:   uses vertices [18..23]
    18, 19, 20, 20, 21, 18,

    // Bottom:  uses vertices [24..29]
    24, 25, 26, 26, 27, 24,

    // Top:     uses vertices [30..35]
    30, 31, 32, 32, 33, 30
  };

  // move data to staging buffer
  vertex_staging_buffer_.CopyData(vertices.data(),
                                  vertices.size() * sizeof(Vertex));

  index_staging_buffer_.CopyData(indices.data(),
                                 indices.size() * sizeof(uint32_t));

  // create the cube
  Mesh cube;
  cube.name = "cube";
  cube.vertex_offset = 0;
  cube.index_offset = 0;
  cube.index_count = 36;

  meshes_.push_back(cube);
}

}  // namespace braque