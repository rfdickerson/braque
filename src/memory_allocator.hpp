//
// Created by Robert F. Dickerson on 12/16/24.
//

#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP
#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

namespace braque {

class Renderer;

    struct AllocatedImage
    {
        vk::Image image;
        VmaAllocation allocation;
    };

class MemoryAllocator {
public:
    MemoryAllocator(Renderer& renderer);
    ~MemoryAllocator();

    // make sure copy and move are deleted
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator=(MemoryAllocator&&) = delete;

    AllocatedImage createImage(const vk::ImageCreateInfo& createInfo, const VmaAllocationCreateInfo& allocInfo);
    void destroyImage(AllocatedImage& image);

private:
    Renderer& renderer;

    VmaAllocator allocator;
};

} // braque

#endif //MEMORY_ALLOCATOR_HPP
