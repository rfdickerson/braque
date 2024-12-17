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

    // make sure copy and move are deleted
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    Swapchain(Swapchain&&) = delete;
    Swapchain& operator=(Swapchain&&) = delete;


    vk::SwapchainKHR getSwapchain() const { return swapchain; }
    vk::Image getSwapchainImage() const { return swapchainImages[currentImageIndex]; }
    vk::CommandBuffer getCommandBuffer() const { return commandBuffers[currentImageIndex]; }
    uint32_t getImageCount() const { return imageCount; }
    vk::ImageView getImageView() const { return swapchainImageViews[currentImageIndex]; }
    vk::Rect2D getExtent() const { return swapchainExtent; }
    vk::Format getFormat() const { return swapchainFormat; }

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
    std::vector<vk::ImageView> swapchainImageViews;
    vk::Rect2D swapchainExtent;
    vk::Format swapchainFormat;

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
    void createImageViews();


};

} // braque

#endif //SWAPCHAIN_HPP
