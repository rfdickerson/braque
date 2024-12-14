//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "window.hpp"
#include "renderer.hpp"

namespace braque {

class Swapchain {
public:
    Swapchain(Window& window, Renderer& renderer);
    ~Swapchain();

private:
    vk::SwapchainKHR swapchain;

    Renderer& renderer;

};

} // braque

#endif //SWAPCHAIN_HPP
