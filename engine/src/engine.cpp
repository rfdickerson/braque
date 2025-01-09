//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.h"

#include "braque/debug_window.h"
#include "braque/memory_allocator.h"
#include "braque/renderer.h"
#include "braque/rendering_stage.h"
#include "braque/swapchain.h"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace braque {

Engine::Engine()
    :
      memoryAllocator(renderer),
context_(memoryAllocator, renderer),
      swapchain(window, context_),
      uniforms_(context_, swapchain),
      renderingStage(context_, swapchain,uniforms_),
      debugWindow(*this),
      scene_(context_, uniforms_) {
  // Any other initialization after all members are constructed
  spdlog::info("Engine created");
  input_controller_.RegisterWindow(&window);
  input_controller_.RegisterObserver(&fps_controller_);
  fps_controller_.SetCamera(&camera_);
  input_controller_.RegisterObserver(&app_controller_);
  app_controller_.SetEngine(this);

  camera_.SetAspectRatio(swapchain.getExtent().width /
                         static_cast<float>(swapchain.getExtent().height));
}

Engine::~Engine() {
  renderer.waitIdle();
}

void Engine::run() {
  // set up the scene
  scene_.UploadSceneData();

  spdlog::info("Starting the engine loop");

  while (running) {

    swapchain.waitForFrame();
    swapchain.acquireNextImage();
    swapchain.waitForImageInFlight();

    auto& frameStats = swapchain.getFrameStats();
    frameStats.Update();

    // get number of ticks to process
    uint32_t ticksToProcess = frameStats.GetTicksToProcess();

    // process fixed timestep updates
    while (ticksToProcess > 0) {
      uint32_t ticksThisIteration = std::min(ticksToProcess, FrameStats::TICKS_240HZ());
      input_controller_.PollEvents();

      frameStats.ConsumeTime(ticksThisIteration);
      ticksToProcess -= ticksThisIteration;
    }

    // sleep for 1 ms to simulate CPU work
    std::this_thread::sleep_for(std::chrono::milliseconds(2));

    debugWindow.createFrame(frameStats);

    auto extent = swapchain.getExtent();

    auto commandBuffer = swapchain.getCommandBuffer();
    RenderingStage::begin(commandBuffer);
    uniforms_.SetCameraData(commandBuffer, camera_);
    renderingStage.prepareImageForColorAttachment(commandBuffer);
    renderingStage.beginRenderingPass(commandBuffer);
    uniforms_.Bind(commandBuffer, renderingStage.GetPipeline().VulkanLayout());
    renderingStage.GetPipeline().Bind(commandBuffer);
    Pipeline::SetScissor(commandBuffer,
                         vk::Rect2D{{0, 0}, {extent.width, extent.height}});
    Pipeline::SetViewport(commandBuffer,
                          {0, 0, static_cast<float>(extent.width),
                           static_cast<float>(extent.height), 0, 1});
    scene_.Draw(commandBuffer);
    //renderingStage.renderTriangle( commandBuffer );
    DebugWindow::renderFrame(commandBuffer);
    RenderingStage::endRenderingPass(commandBuffer);
    renderingStage.prepareImageForDisplay(commandBuffer);
    RenderingStage::end(commandBuffer);

    swapchain.submitCommandBuffer();



    swapchain.presentImage();
  }
}

}  // namespace braque