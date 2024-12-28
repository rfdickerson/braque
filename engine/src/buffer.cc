//
// Created by Robert F. Dickerson on 12/28/24.
//

#include "braque/buffer.h"

namespace braque {

Buffer::Buffer(Engine& engine, vk::BufferCreateInfo buffer_create_info, VmaAllocationCreateInfo allocation_info) : type_(BufferType::staging), size_(buffer_create_info.size), buffer_(nullptr), allocation_(nullptr), engine_(engine) {

  const auto allocatedBuffer = engine_.getMemoryAllocator().createBuffer(buffer_create_info, allocation_info);
  buffer_ = allocatedBuffer.buffer;
  allocation_ = allocatedBuffer.allocation;

}

Buffer::~Buffer() {
  engine_.getMemoryAllocator().destroyBuffer(*this);
}

auto Buffer::GetAllocation() const -> VmaAllocation {
  return allocation_;
}

auto Buffer::GetBuffer() const -> vk::Buffer {
  return buffer_;
}


}
