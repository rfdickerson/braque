//
// Created by Robert F. Dickerson on 12/27/24.
//

#include "braque/scene.h"

namespace braque {
Scene::Scene(MemoryAllocator& allocator, Renderer& renderer)
    : allocator_(allocator), renderer_(renderer) {
  CreateVertexBuffer();
  CreateIndexBuffer();
  CreateStagingBuffer();
}

void Scene::CreateVertexBuffer() {
  // create the vertex buffer
  vk::BufferCreateInfo vertexBufferCreateInfo{};
  vertexBufferCreateInfo.setSize(65536);
  vertexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer |
                                  vk::BufferUsageFlagBits::eTransferDst);

  VmaAllocationCreateInfo vertexBufferAllocInfo{};
  vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
  vertexBufferAllocInfo.flags = 0;

  vertex_buffer_ =
      allocator_.createBuffer(vertexBufferCreateInfo, vertexBufferAllocInfo);
}

void Scene::CreateIndexBuffer() {
  // create the index buffer
  vk::BufferCreateInfo indexBufferCreateInfo;
  indexBufferCreateInfo.setSize(65536);
  indexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
                                 vk::BufferUsageFlagBits::eTransferDst);

  VmaAllocationCreateInfo indexBufferAllocInfo{};
  indexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
  indexBufferAllocInfo.flags = 0;

  index_buffer_ =
      allocator_.createBuffer(indexBufferCreateInfo, indexBufferAllocInfo);
}

void Scene::CreateStagingBuffer() {
  // create the staging buffer
  vk::BufferCreateInfo stagingBufferCreateInfo;
  stagingBufferCreateInfo.setSize(kVertexBufferSize);
  stagingBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
  stagingBufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo stagingBufferAllocInfo{};
  stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  stagingBufferAllocInfo.flags =
      VMA_ALLOCATION_CREATE_MAPPED_BIT |
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  vertex_staging_buffer_ =
      allocator_.createBuffer(stagingBufferCreateInfo, stagingBufferAllocInfo);
  index_staging_buffer_ =
      allocator_.createBuffer(stagingBufferCreateInfo, stagingBufferAllocInfo);
}

Scene::~Scene() {
  allocator_.destroyBuffer(vertex_buffer_);
  allocator_.destroyBuffer(index_buffer_);
  allocator_.destroyBuffer(vertex_staging_buffer_);
  allocator_.destroyBuffer(index_staging_buffer_);
}

void Scene::Draw(vk::CommandBuffer buffer) {
  // bind the vertex buffer
  buffer.bindVertexBuffers(0, vertex_buffer_.buffer, {0});
  buffer.bindIndexBuffer(index_buffer_.buffer, 0, vk::IndexType::eUint32);

  for (const auto& mesh : meshes_) {
    // draw the mesh
    buffer.drawIndexed(mesh.index_count, 1, mesh.index_offset,
                       mesh.vertex_offset, 0);
  }
}

void Scene::UploadSceneData(vk::CommandBuffer buffer) {
  // begin single time command buffer
  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  buffer.begin(beginInfo);
  // transfer the data from staging buffer to vertex buffer
  vk::BufferCopy copyRegion;
  copyRegion.setSize(kVertexBufferSize);
  buffer.copyBuffer(vertex_staging_buffer_.buffer, vertex_buffer_.buffer,
                    copyRegion);

  // transfer the data from staging buffer to index buffer
  copyRegion.setSize(kVertexBufferSize);
  buffer.copyBuffer(index_staging_buffer_.buffer, index_buffer_.buffer,
                    copyRegion);

  // end the command buffer
  buffer.end();

  // submit
  vk::SubmitInfo submitInfo;
  submitInfo.setCommandBuffers(buffer);
  renderer_.getGraphicsQueue().submit(submitInfo, nullptr);
}

void Scene::AddCube(glm::vec3 position) {

  // create the vertices
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}},
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

        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}}
      };

  std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8
  };

  // move data to staging buffer


  // create the cube
  Mesh cube;
  cube.name = "cube";
  cube.vertex_offset = 0;
  cube.index_offset = 0;
  cube.index_count = 36;

  meshes_.push_back(cube);
}

}  // namespace braque