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

  // create the vertices
  std::vector<Vertex> vertices = {{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
                                  {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
                                  {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
                                  {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
                                  {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
                                  {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},

                                  {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                  {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

                                  {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}}};

  std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0,  4,  5,  6,
                                   6, 7, 4, 8, 9, 10, 10, 11, 8};

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