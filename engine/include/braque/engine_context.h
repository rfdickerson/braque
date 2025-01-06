//
// Created by Robert F. Dickerson on 1/6/25.
//

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H

#include "memory_allocator.h"
#include "renderer.h"
#include "swapchain.h"

namespace braque {

class EngineContext {
 public:
  EngineContext(MemoryAllocator& allocator, Renderer& renderer,
                Swapchain& swapchain)
      : allocator_(allocator), renderer_(renderer), swapchain_(swapchain) {}
  MemoryAllocator& getMemoryAllocator() { return allocator_; }
  Renderer& getRenderer() { return renderer_; }
  Swapchain& getSwapchain() { return swapchain_; }

 private:
  MemoryAllocator& allocator_;
  Renderer& renderer_;
  Swapchain& swapchain_;
};

}  // namespace braque
#endif  //ENGINE_CONTEXT_H
