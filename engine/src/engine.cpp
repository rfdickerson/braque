//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.hpp"

#include <spdlog/spdlog.h>

#include "braque/window.hpp"
#include "braque/renderer.hpp"
#include "braque/swapchain.hpp"
#include "braque/rendering_stage.hpp"
#include "braque/debug_window.hpp"
#include "braque/memory_allocator.hpp"

namespace braque {

    Engine::Engine() :
        swapchain(Swapchain(window, renderer)),
        memoryAllocator(MemoryAllocator(renderer)),
        renderingStage(RenderingStage(*this)),
        debugWindow(DebugWindow(*this))
    {
        // Any other initialization after all members are constructed
    }

Engine::~Engine() {

    renderer.waitIdle();
}

void Engine::run() {

    spdlog::info("Starting the engine loop");

    while (!window.shouldClose()) {
        Window::pollEvents();

        swapchain.waitForFrame();
        swapchain.acquireNextImage();
        swapchain.waitForImageInFlight();
        // do drawing here

        debugWindow.createFrame();

        auto commandBuffer = swapchain.getCommandBuffer();
        renderingStage.begin(commandBuffer);
        renderingStage.prepareImageForColorAttachment(commandBuffer);
        renderingStage.beginRenderingPass(commandBuffer);
        DebugWindow::renderFrame(commandBuffer);
        renderingStage.endRenderingPass(commandBuffer);
        renderingStage.prepareImageForDisplay(commandBuffer);
        renderingStage.end(commandBuffer);

        swapchain.submitCommandBuffer();
        swapchain.presentImage();
    }
}


} // braque