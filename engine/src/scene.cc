//
// Created by Robert F. Dickerson on 12/27/24.
//

#include "braque/scene.h"

namespace braque {
Scene::Scene(MemoryAllocator& allocator, Renderer& renderer)
    : allocator_(allocator), renderer_(renderer) {
  CreateVertexBuffer();
  // CreateIndexBuffer();
  // CreateStagingBuffer();
}

void Scene::CreateVertexBuffer() {
    // create the vertex buffer
    vk::BufferCreateInfo vertexBufferCreateInfo {};
    vertexBufferCreateInfo.setSize(65536);
    vertexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eVertexBuffer |
                                    vk::BufferUsageFlagBits::eTransferDst);

    VmaAllocationCreateInfo vertexBufferAllocInfo;
    vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    vertex_buffer_ =
        allocator_.createBuffer(vertexBufferCreateInfo, vertexBufferAllocInfo);

}

void Scene::CreateIndexBuffer() {
  // create the index buffer
  vk::BufferCreateInfo indexBufferCreateInfo;
  indexBufferCreateInfo.setSize(3 * sizeof(uint32_t));
  indexBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
                                 vk::BufferUsageFlagBits::eTransferDst);
  indexBufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo indexBufferAllocInfo;
  indexBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

  index_buffer_ =
      allocator_.createBuffer(indexBufferCreateInfo, indexBufferAllocInfo);
}

void Scene::CreateStagingBuffer() {
  // create the staging buffer
  vk::BufferCreateInfo stagingBufferCreateInfo;
  stagingBufferCreateInfo.setSize(kVertexBufferSize);
  stagingBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
  stagingBufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo stagingBufferAllocInfo;
  stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
  stagingBufferAllocInfo.flags =
      VMA_ALLOCATION_CREATE_MAPPED_BIT |
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  staging_buffer_ =
      allocator_.createBuffer(stagingBufferCreateInfo, stagingBufferAllocInfo);
}

Scene::~Scene() {
  allocator_.destroyBuffer(vertex_buffer_);
  allocator_.destroyBuffer(index_buffer_);
  allocator_.destroyBuffer(staging_buffer_);
}

}  // namespace braque