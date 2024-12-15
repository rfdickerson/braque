//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "window.hpp"
#include "renderer.hpp"

namespace braque {

const int MAX_FRAMES_IN_FLIGHT = 2;

class Swapchain {
public:
    Swapchain(Window& window, Renderer& renderer);
    ~Swapchain();

    vk::SwapchainKHR getSwapchain() const { return swapchain; }
    vk::Image getSwapchainImage() const { return swapchainImages[currentImageIndex]; }
    vk::CommandBuffer getCommandBuffer() const { return commandBuffers[currentImageIndex]; }

    void waitForFrame() const;
    void acquireNextImage();
    void waitForImageInFlight();
    void submitCommandBuffer();
    void presentImage();

private:
    vk::SwapchainKHR swapchain;
    vk::SurfaceKHR surface;

    Renderer& renderer;

    uint32_t imageCount = 2;
    uint32_t currentImageIndex = 0;
    uint32_t currentFrameInFlight = 0;
    std::vector<vk::Image> swapchainImages;

    // per swapchain image
    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;

    // per frame in flight
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;

    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

    void createSwapchain(Window& window);
    void createSemaphores();
    void createFences();
    void createSwapchainImages();
    void createCommandBuffers();


};

} // braque

#endif //SWAPCHAIN_HPP
