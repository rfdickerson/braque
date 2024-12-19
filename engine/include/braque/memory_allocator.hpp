//
// Created by Robert F. Dickerson on 12/16/24.
//

#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP
#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>
#include "renderer.hpp"

namespace braque {
struct AllocatedImage {
    vk::Image image;
    VmaAllocation allocation;
};

struct MemoryReport {
    uint32_t allocations;
    vk::DeviceSize totalMemory;
    vk::DeviceSize usedMemory;
    vk::DeviceSize freeMemory;
};

class MemoryAllocator {
public:
    explicit MemoryAllocator(const Renderer &renderer);
    ~MemoryAllocator();

    // make sure copy and move are deleted
    MemoryAllocator(const MemoryAllocator &) = delete;
    auto operator=(const MemoryAllocator &) -> MemoryAllocator & = delete;
    MemoryAllocator(MemoryAllocator &&) = delete;
    auto operator=(MemoryAllocator &&) -> MemoryAllocator & = delete;

    auto getReport() const -> MemoryReport;

    [[nodiscard]] auto createImage(const vk::ImageCreateInfo &createInfo, const VmaAllocationCreateInfo &allocInfo) const -> AllocatedImage;

    void destroyImage(const AllocatedImage &image) const;

private:
    VmaAllocator allocator;
};
} // namespace braque

#endif // MEMORY_ALLOCATOR_HPP
