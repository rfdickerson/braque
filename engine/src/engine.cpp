//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.h"

#include "braque/debug_window.h"
#include "braque/memory_allocator.h"
#include "braque/renderer.h"
#include "braque/rendering_stage.h"
#include "braque/swapchain.h"
#include "braque/asset_loader.h"

#include <spdlog/spdlog.h>


namespace braque {

Engine::Engine()
    :
      memoryAllocator(renderer),
context_(memoryAllocator, renderer),
      swapchain(window, context_),
      uniforms_(context_, swapchain),
      renderingStage(context_, swapchain,uniforms_, assetLoader_),
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
    // std::this_thread::sleep_for(std::chrono::milliseconds(2));

    debugWindow.createFrame(frameStats);

    SyncBarriers barriers;
    auto extent = swapchain.getExtent();
    auto& swapchainImage = swapchain.GetSwapchainImage();
    auto& currentColorImage = renderingStage.GetColorImages()[swapchain.CurrentFrameIndex()];
    auto& currentPostprocessImage = renderingStage.GetPostprocessingImages()[swapchain.CurrentFrameIndex()];

    auto commandBuffer = swapchain.getCommandBuffer();
    RenderingStage::begin(commandBuffer);
    uniforms_.SetCameraData(commandBuffer, camera_);

    // prepare color image for rendering to

    barriers.srcStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.srcAccess = {};
    barriers.dstStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.dstAccess = vk::AccessFlagBits2::eColorAttachmentWrite;

    currentColorImage.TransitionLayout(vk::ImageLayout::eColorAttachmentOptimal, commandBuffer, barriers);

    renderingStage.beginRenderingPass(commandBuffer);
    uniforms_.Bind(commandBuffer, renderingStage.GetPipeline().VulkanLayout());
    renderingStage.GetPipeline().Bind(commandBuffer);
    Pipeline::SetScissor(commandBuffer,
                         vk::Rect2D{{0, 0}, {extent.width, extent.height}});
    Pipeline::SetViewport(commandBuffer,
                          {0, 0, static_cast<float>(extent.width),
                           static_cast<float>(extent.height), 0, 1});
    scene_.Draw(commandBuffer);

    //DebugWindow::renderFrame(commandBuffer);

    RenderingStage::endRenderingPass(commandBuffer);

    // transition color image to transfer src
    barriers.srcStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.srcAccess = vk::AccessFlagBits2::eColorAttachmentWrite;
    barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
    barriers.dstAccess = vk::AccessFlagBits2::eTransferRead;
    currentColorImage.TransitionLayout(vk::ImageLayout::eTransferSrcOptimal, commandBuffer, barriers);

    // transition postprocess image to transfer dst optimal
    barriers.srcStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.srcAccess = vk::AccessFlagBits2::eColorAttachmentWrite;
    barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
    barriers.dstAccess = vk::AccessFlagBits2::eTransferWrite;
    currentPostprocessImage.TransitionLayout(vk::ImageLayout::eTransferDstOptimal, commandBuffer, barriers);

    // resolve
    currentColorImage.ResolveImage(commandBuffer, currentPostprocessImage);

    // transition postprocess image to transfer src optimal
    barriers.srcStage = vk::PipelineStageFlagBits2::eResolve;
    barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
    barriers.dstStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.dstAccess = vk::AccessFlagBits2::eColorAttachmentRead;
    currentPostprocessImage.TransitionLayout(vk::ImageLayout::eColorAttachmentOptimal, commandBuffer, barriers);

    // render the debug window
    debugWindow.BeginRendering(commandBuffer, currentPostprocessImage);

    // transition post process image to transfer src optimal
    barriers.srcStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.srcAccess = vk::AccessFlagBits2::eColorAttachmentWrite;
    barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
    barriers.dstAccess = vk::AccessFlagBits2::eTransferRead;
    currentPostprocessImage.TransitionLayout(vk::ImageLayout::eTransferSrcOptimal, commandBuffer, barriers);

    // transition swapchain image to transfer dst
    barriers.srcStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput | vk::PipelineStageFlagBits2::eBottomOfPipe;
    barriers.srcAccess = vk::AccessFlagBits2::eColorAttachmentRead;
    barriers.dstStage = vk::PipelineStageFlagBits2::eTransfer;
    barriers.dstAccess = vk::AccessFlagBits2::eTransferWrite;
    swapchainImage.TransitionLayout(vk::ImageLayout::eTransferDstOptimal, commandBuffer, barriers);

    // Blit image
    currentPostprocessImage.BlitImage(commandBuffer, swapchainImage);

    // transition swapchain image to present
    barriers.srcStage = vk::PipelineStageFlagBits2::eTransfer;
    barriers.srcAccess = vk::AccessFlagBits2::eTransferWrite;
    barriers.dstStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
    barriers.dstAccess = {};
    swapchainImage.TransitionLayout(vk::ImageLayout::ePresentSrcKHR, commandBuffer, barriers);

    RenderingStage::end(commandBuffer);

    swapchain.submitCommandBuffer();



    swapchain.presentImage();
  }
}

}  // namespace braque