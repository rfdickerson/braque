//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "camera.h"
#include "debug_window.h"
#include "engine_context.h"
#include "input/app_controller.h"
#include "input/fps_controller.h"
#include "input/input_controller.h"
#include "memory_allocator.h"
#include "renderer.h"
#include "rendering_stage.h"
#include "scene.h"
#include "swapchain.h"
#include "uniforms.h"
#include "window.h"

namespace braque {

class Engine {
 public:
  Engine();
  ~Engine();

  // Make sure copy and move are deleted
  Engine(const Engine& other) = delete;
  Engine(Engine&& other) noexcept = delete;
  auto operator=(const Engine& other) -> Engine& = delete;
  auto operator=(Engine&& other) noexcept -> Engine& = delete;

  auto getRenderer() -> Renderer& { return renderer; }

  auto getWindow() -> Window& { return window; }

  auto getSwapchain() -> Swapchain& { return swapchain; }

  //auto getRenderingStage() -> RenderingStage& { return renderingStage; }
  auto getRenderingStage() -> RenderingStage& { return renderingStage; }

  auto getMemoryAllocator() -> MemoryAllocator& { return memoryAllocator; }

  auto getUniforms() -> Uniforms& { return uniforms_; }

  void Quit() { running = false; }

  void run();

 private:
  Window window;
  Renderer renderer;
  MemoryAllocator memoryAllocator;
  EngineContext context_;
  Swapchain swapchain;
  Uniforms uniforms_;
  Camera camera_;
  RenderingStage renderingStage;
  DebugWindow debugWindow;
  InputController input_controller_;
  FirstPersonController fps_controller_;
  AppController app_controller_;
  Scene scene_;

  bool running = true;


};

}  // namespace braque

#endif  // ENGINE_HPP
