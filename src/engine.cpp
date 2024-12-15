//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "renderer.hpp"
#include "window.hpp"
#include "swapchain.hpp"
#include "rendering_stage.hpp"

namespace braque {

Engine::Engine() {
    window = new Window();
    renderer = new Renderer();
    swapchain = new Swapchain(*window, *renderer);
    renderingStage = new RenderingStage(*renderer, *swapchain);
}

Engine::~Engine() {

    delete renderingStage;
    delete swapchain;
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

        renderingStage->render();

        swapchain->submitCommandBuffer();
        swapchain->presentImage();
    }
}


} // braque