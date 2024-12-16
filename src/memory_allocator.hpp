//
// Created by Robert F. Dickerson on 12/16/24.
//

#ifndef MEMORY_ALLOCATOR_HPP
#define MEMORY_ALLOCATOR_HPP
#include <vk_mem_alloc.h>

namespace braque {

class Renderer;

class MemoryAllocator {
public:
    MemoryAllocator(Renderer& renderer);
    ~MemoryAllocator();

private:
    Renderer& renderer;

    VmaAllocator allocator;
};

} // braque

#endif //MEMORY_ALLOCATOR_HPP
