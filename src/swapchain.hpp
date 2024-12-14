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

    vk::SwapchainKHR getSwapchain() const { return swapchain; }
    vk::Image getSwapchainImage() const { return swapchainImages[0]; }

    uint32_t acquireNextImage();
    void presentImage();

private:
    vk::SwapchainKHR swapchain;
    vk::SurfaceKHR surface;

    Renderer& renderer;

    uint32_t currentImageIndex = 0;
    uint32_t currentFrameInFlight = 0;
    std::vector<vk::Image> swapchainImages;




};

} // braque

#endif //SWAPCHAIN_HPP
