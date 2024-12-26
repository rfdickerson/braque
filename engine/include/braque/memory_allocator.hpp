//
// Created by Robert F. Dickerson on 12/16/24.
//

#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP

#include "renderer.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace braque
{
  struct AllocatedBuffer
  {
    vk::Buffer    buffer;
    VmaAllocation allocation;
    // mapped data
    void* mappedData;
  };

  struct AllocatedImage
  {
    vk::Image     image;
    VmaAllocation allocation;
  };

  struct MemoryReport
  {
    uint32_t       allocations;
    vk::DeviceSize totalMemory;
    vk::DeviceSize usedMemory;
    vk::DeviceSize freeMemory;
  };

  class MemoryAllocator
  {
  public:
    explicit MemoryAllocator( const Renderer & renderer );
    ~MemoryAllocator();

    // make sure copy and move are deleted
    MemoryAllocator( const MemoryAllocator & )                     = delete;
    auto operator=( const MemoryAllocator & ) -> MemoryAllocator & = delete;
    MemoryAllocator( MemoryAllocator && )                          = delete;
    auto operator=( MemoryAllocator && ) -> MemoryAllocator &      = delete;

    [[nodiscard]] auto getReport() const -> MemoryReport;

    [[nodiscard]] auto createImage( const vk::ImageCreateInfo & createInfo, const VmaAllocationCreateInfo & allocInfo ) const -> AllocatedImage;
    [[nodiscard]] auto createBuffer( const vk::BufferCreateInfo & createInfo, const VmaAllocationCreateInfo & allocInfo ) const -> AllocatedBuffer;

    void destroyImage( const AllocatedImage & image ) const;
    void destroyBuffer( const AllocatedBuffer & buffer ) const;

  private:
    VmaAllocator allocator;
  };
}  // namespace braque

#endif  // MEMORY_ALLOCATOR_HPP
