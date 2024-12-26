//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "debug_window.hpp"
#include "memory_allocator.hpp"
#include "renderer.hpp"
#include "rendering_stage.hpp"
#include "swapchain.hpp"
#include "uniforms.hpp"
#include "window.hpp"

namespace braque
{

  class Engine
  {
  public:
    Engine();
    ~Engine();

    // Make sure copy and move are deleted
    Engine( const Engine & other )                         = delete;
    Engine( Engine && other ) noexcept                     = delete;
    auto operator=( const Engine & other ) -> Engine &     = delete;
    auto operator=( Engine && other ) noexcept -> Engine & = delete;

    auto getRenderer() -> Renderer &
    {
      return renderer;
    }

    auto getWindow() -> Window &
    {
      return window;
    }

    auto getSwapchain() -> Swapchain &
    {
      return swapchain;
    }

    auto getRenderingStage() -> RenderingStage &
    {
      return renderingStage;
    }

    auto getMemoryAllocator() -> MemoryAllocator &
    {
      return memoryAllocator;
    }

    void run();

  private:
    Window          window;
    Renderer        renderer;
    Swapchain       swapchain;
    MemoryAllocator memoryAllocator;
    RenderingStage  renderingStage;
    DebugWindow     debugWindow;
    Uniforms        uniforms_;
  };

}  // namespace braque

#endif  // ENGINE_HPP
