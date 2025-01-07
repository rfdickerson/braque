//
// Created by Robert F. Dickerson on 1/6/25.
//

#ifndef ENGINE_CONTEXT_H
#define ENGINE_CONTEXT_H


namespace braque {

class MemoryAllocator;
class Renderer;
class Swapchain;

class EngineContext {
 public:
  EngineContext(MemoryAllocator& allocator, Renderer& renderer)
      : allocator_(allocator), renderer_(renderer) {}
  auto getMemoryAllocator() const -> MemoryAllocator& { return allocator_; }
  auto getRenderer() const -> Renderer& { return renderer_; }
  // auto getSwapchain() const -> Swapchain& { return swapchain_; }

 private:
  MemoryAllocator& allocator_;
  Renderer& renderer_;
  // Swapchain& swapchain_;
};

}  // namespace braque
#endif  //ENGINE_CONTEXT_H
