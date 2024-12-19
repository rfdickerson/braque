//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <memory>

#include "renderer.hpp"
#include "window.hpp"
#include "swapchain.hpp"
#include "rendering_stage.hpp"
#include "debug_window.hpp"
#include "memory_allocator.hpp"

namespace braque {

class Engine {
public:
    Engine();
    ~Engine();

    Renderer& getRenderer() { return renderer; }
    Window& getWindow() { return window; }
    Swapchain& getSwapchain() { return swapchain; }
    RenderingStage& getRenderingStage() { return renderingStage; }
    MemoryAllocator& getMemoryAllocator() { return memoryAllocator; }

    void run();

private:
    Window window;
    Renderer renderer;
    Swapchain swapchain;
    MemoryAllocator memoryAllocator;
    RenderingStage renderingStage;
    DebugWindow debugWindow;

};

} // braque

#endif //ENGINE_HPP