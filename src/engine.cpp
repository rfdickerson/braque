//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "engine.hpp"

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
    while (!window->shouldClose()) {
        window->pollEvents();
    }
}


} // braque