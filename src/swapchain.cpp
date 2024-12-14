//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "swapchain.hpp"

namespace braque {

Swapchain::Swapchain(Window& window, Renderer& renderer) {

    auto surface = window.createSurface(renderer);
    

}

Swapchain::~Swapchain() {
}

} // braque