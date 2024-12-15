//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "engine.hpp"

#include <spdlog/spdlog.h>

#include "renderer.hpp"
#include "window.hpp"
#include "swapchain.hpp"

namespace braque {

Engine::Engine() {
    window = new Window();
    renderer = new Renderer();
    swapchain = new Swapchain(*window, *renderer);
}

Engine::~Engine() {

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

        auto buffer = swapchain->getCommandBuffer();
        buffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        buffer.end();

        swapchain->submitCommandBuffer();
        swapchain->presentImage();
    }
}


} // braque