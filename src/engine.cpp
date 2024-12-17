//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "window.hpp"
#include "renderer.hpp"
#include "swapchain.hpp"
#include "rendering_stage.hpp"
#include "debug_window.hpp"
#include "memory_allocator.hpp"

namespace braque {

Engine::Engine() {
    window = new Window();
    renderer = new Renderer();
    memoryAllocator = new MemoryAllocator(*renderer);
    swapchain = new Swapchain(*window, *renderer);
    renderingStage = new RenderingStage(*renderer, *swapchain);
    debugWindow = new DebugWindow(*this);
}

Engine::~Engine() {

    renderer->waitIdle();
    delete debugWindow;
    delete renderingStage;
    delete swapchain;
    delete memoryAllocator;
    delete renderer;
    delete window;
}

void Engine::run() {

    spdlog::info("Starting the engine loop");

    while (!window->shouldClose()) {
        window->pollEvents();

        swapchain->waitForFrame();
        swapchain->acquireNextImage();
        swapchain->waitForImageInFlight();
        // do drawing here

        debugWindow->createFrame();

        auto commandBuffer = swapchain->getCommandBuffer();
        renderingStage->begin(commandBuffer);
        renderingStage->prepareImageForColorAttachment(commandBuffer);
        renderingStage->beginRenderingPass(commandBuffer);
        debugWindow->renderFrame(commandBuffer);
        renderingStage->endRenderingPass(commandBuffer);
        renderingStage->prepareImageForDisplay(commandBuffer);
        renderingStage->end(commandBuffer);

        swapchain->submitCommandBuffer();
        swapchain->presentImage();
    }
}


} // braque