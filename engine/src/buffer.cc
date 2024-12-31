//
// Created by Robert F. Dickerson on 12/28/24.
//

#include "braque/buffer.h"
#include "braque/engine.h"

#include <spdlog/spdlog.h>

namespace braque {

Buffer::Buffer(Engine& engine, BufferType buffer_type, vk::DeviceSize size)
    : type_(buffer_type),
      size_(size),
      buffer_(nullptr),
      allocation_(nullptr),
      engine_(engine) {

  vk::BufferCreateInfo buffer_create_info;
  buffer_create_info.setSize(size);

  VmaAllocationCreateInfo allocation_info{};
  // allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
  // allocation_info.flags = 0;

  switch (buffer_type) {
    case BufferType::vertex:
      buffer_create_info.setUsage(vk::BufferUsageFlagBits::eVertexBuffer |
                                  vk::BufferUsageFlagBits::eTransferDst);
      allocation_info.usage = VMA_MEMORY_USAGE_AUTO;
      break;
    case BufferType::index:
      buffer_create_info.setUsage(vk::BufferUsageFlagBits::eIndexBuffer |
                                  vk::BufferUsageFlagBits::eTransferDst);
      allocation_info.usage = VMA_MEMORY_USAGE_AUTO;
      break;
    case BufferType::uniform:
      buffer_create_info.setUsage(vk::BufferUsageFlagBits::eUniformBuffer |
                                  vk::BufferUsageFlagBits::eTransferDst);
      allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
      allocation_info.flags =
          VMA_ALLOCATION_CREATE_MAPPED_BIT |
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
          VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
      break;
    case BufferType::staging:
      buffer_create_info.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
      allocation_info.usage = VMA_MEMORY_USAGE_AUTO;
    allocation_info.flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
      VMA_ALLOCATION_CREATE_MAPPED_BIT;

      break;
    default:
      spdlog::warn("Buffer type not recognized");
  }

  buffer_create_info.setSharingMode(vk::SharingMode::eExclusive);

  VkBufferCreateInfo vk_create_info = buffer_create_info;
  VkBuffer buffer = nullptr;

  const auto allocator = engine_.getMemoryAllocator().getAllocator();
  vmaCreateBuffer(allocator, &vk_create_info, &allocation_info, &buffer,
                  &allocation_, &allocation_info_);

  buffer_ = buffer;

  spdlog::info("Created buffer");
}

Buffer::Buffer(Engine& engine, vk::BufferCreateInfo buffer_create_info,
               VmaAllocationCreateInfo allocation_info)
    : type_(BufferType::staging),
      size_(buffer_create_info.size),
      buffer_(nullptr),
      allocation_(nullptr),
      engine_(engine) {

  const auto allocatedBuffer = engine_.getMemoryAllocator().createBuffer(
      buffer_create_info, allocation_info);
  buffer_ = allocatedBuffer.buffer;
  allocation_ = allocatedBuffer.allocation;
}

Buffer::Buffer(Buffer&& other) noexcept
    : type_(other.type_),
      size_(other.size_),
      buffer_(other.buffer_),
      allocation_(other.allocation_),
      engine_(other.engine_) {
  other.buffer_ = nullptr;
  other.allocation_ = nullptr;
  spdlog::info("Copied buffer");
}

Buffer::~Buffer() {
  if (buffer_ != nullptr) {
    engine_.getMemoryAllocator().destroyBuffer(*this);
    spdlog::info("Destroyed the buffer");
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

void Buffer::CopyData(const void* data, size_t size) {
  auto const allocator = engine_.getMemoryAllocator().getAllocator();

  vmaMapMemory(allocator, allocation_, &mapped_data_);
  std::memcpy(mapped_data_, data, size);
  vmaUnmapMemory(allocator, allocation_);
}

void Buffer::CopyData(vk::CommandBuffer buffer, const void* data, size_t size) {
  auto const allocator = engine_.getMemoryAllocator().getAllocator();

  std::memcpy(allocation_info_.pMappedData, data, size);

  const auto result =
      vmaFlushAllocation(allocator, allocation_, 0, VK_WHOLE_SIZE);

  if (result != VK_SUCCESS) {
    spdlog::error("Failed to flush memory");
  }

  vk::BufferMemoryBarrier barrier;
  barrier.setBuffer(buffer_);
  barrier.setSize(VK_WHOLE_SIZE);
  barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
  barrier.setDstAccessMask(vk::AccessFlagBits::eUniformRead);
  barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
  barrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);

  buffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost,
                         vk::PipelineStageFlagBits::eVertexShader,
                         vk::DependencyFlagBits::eByRegion, nullptr, barrier,
                         nullptr);
}

void Buffer::CopyToBuffer(vk::CommandBuffer buffer, Buffer& destination) {
  vk::BufferCopy copyRegion;
  copyRegion.setSize(size_);
  buffer.copyBuffer(buffer_, destination.GetBuffer(), copyRegion);
}

}  // namespace braque
