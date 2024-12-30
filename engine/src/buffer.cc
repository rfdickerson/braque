//
// Created by Robert F. Dickerson on 12/28/24.
//

#include "braque/buffer.h"
#include "braque/engine.h"

#include <spdlog/spdlog.h>

namespace braque {

Buffer::Buffer(Engine& engine, BufferType buffer_type, vk::DeviceSize size) : type_(buffer_type), size_(size), buffer_(nullptr), allocation_(nullptr), engine_(engine) {
  vk::BufferCreateInfo buffer_create_info;
  buffer_create_info.setSize(size);
  buffer_create_info.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
  buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo allocation_info{};
  allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
  allocation_info.flags = 0;

  const auto allocatedBuffer = engine_.getMemoryAllocator().createBuffer(buffer_create_info, allocation_info);
  buffer_ = allocatedBuffer.buffer;
  allocation_ = allocatedBuffer.allocation;

    spdlog::info("Created buffer");

}

Buffer::Buffer(Engine& engine, vk::BufferCreateInfo buffer_create_info, VmaAllocationCreateInfo allocation_info) : type_(BufferType::staging), size_(buffer_create_info.size), buffer_(nullptr), allocation_(nullptr), engine_(engine) {

  const auto allocatedBuffer = engine_.getMemoryAllocator().createBuffer(buffer_create_info, allocation_info);
  buffer_ = allocatedBuffer.buffer;
  allocation_ = allocatedBuffer.allocation;

}

Buffer::Buffer(Buffer&& other) noexcept : type_(other.type_), size_(other.size_), buffer_(other.buffer_), allocation_(other.allocation_), engine_(other.engine_) {
  other.buffer_ = nullptr;
  other.allocation_ = nullptr;
  spdlog::info("Copied buffer");
}

Buffer::~Buffer() {
  if (buffer_ != nullptr) {
    engine_.getMemoryAllocator().destroyBuffer(*this);
  }
}

auto Buffer::GetAllocation() const -> VmaAllocation {
  return allocation_;
}

auto Buffer::GetBuffer() const -> vk::Buffer {
  return buffer_;
}

auto Buffer::GetSize() const -> vk::DeviceSize {
  return size_;
}

auto Buffer::GetType() const -> BufferType {
  return type_;
}

void Buffer::Bind(vk::CommandBuffer buffer, vk::DeviceSize offset) {
  // do nothing
  buffer.bindVertexBuffers(0, buffer_, {offset});
}


}
