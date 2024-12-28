//
// Created by Robert F. Dickerson on 12/28/24.
//

#ifndef BUFFER_H
#define BUFFER_H
#include <vulkan/vulkan.hpp>

#include "memory_allocator.h"

namespace braque {
  enum class BufferType {
    vertex,
    index,
    uniform,
    staging
  };

class Buffer {
public:
  Buffer();
  ~Buffer();

  Buffer(const Buffer& other) = delete;
  Buffer(Buffer&& other) noexcept = delete;
  Buffer& operator=(const Buffer& other) = delete;
  Buffer& operator=(Buffer&& other) noexcept = delete;

  void Bind();
  void CopyData(const void* data, size_t size);
  void Copy(vk::CommandBuffer, Buffer& destination);

  [[nodiscard]] auto GetBuffer() const -> vk::Buffer;
  [[nodiscard]] auto GetSize() const -> vk::DeviceSize;
  [[nodiscard]] auto GetType() const -> BufferType;

private:
  BufferType type_;
  vk::DeviceSize size_;
  vk::Buffer buffer_;
  VmaAllocation allocation_;

  Renderer& renderer_;
  MemoryAllocator& allocator_;
};
} // namespace braque

#endif //BUFFER_H
