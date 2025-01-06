//
// Created by Robert F. Dickerson on 12/28/24.
//

#ifndef BUFFER_H
#define BUFFER_H
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"

namespace braque {
class EngineContext;

enum class BufferType: uint8_t {
    vertex,
    index,
    uniform,
    staging
  };

class Buffer {
public:
  explicit Buffer(EngineContext& engine, BufferType buffer_type, vk::DeviceSize size);
  explicit Buffer(EngineContext& engine, vk::BufferCreateInfo buffer_create_info, VmaAllocationCreateInfo allocation_info);
  ~Buffer();

  // move constructor
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

  Buffer(Buffer&& other) noexcept;
  // Buffer& operator=(Buffer&&) noexcept;

  void Bind(vk::CommandBuffer buffer, vk::DeviceSize offset = 0);
  void CopyData(const void* data, size_t size);
  void CopyData(vk::CommandBuffer buffer, const void* data, size_t size);
  void CopyToBuffer(vk::CommandBuffer, Buffer& destination);

  [[nodiscard]] auto GetBuffer() const -> vk::Buffer;
  [[nodiscard]] auto GetAllocation() const -> VmaAllocation;
  [[nodiscard]] auto GetSize() const -> vk::DeviceSize;
  [[nodiscard]] auto GetType() const -> BufferType;

  template <typename T>
  auto GetPointer() const -> T* {
    return static_cast<T*>(allocation_info_.pMappedData);
  }

private:
  BufferType type_;
  vk::DeviceSize size_;
  vk::Buffer buffer_;
  VmaAllocation allocation_;
  VmaAllocationInfo allocation_info_;

  bool is_persistently_mapped_ = false;
  bool is_mapped_ = false;
  void* mapped_data_ = nullptr;

  EngineContext& engine_;
};
} // namespace braque

#endif //BUFFER_H
