//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/swapchain.hpp"

#include <spdlog/spdlog.h>

namespace braque {
Swapchain::Swapchain(Window &window, Renderer &renderer) : renderer(renderer), swapchainFormat(vk::Format::eUndefined) {
    createSwapchain(window);
    createSemaphores();
    createFences();
    createSwapchainImages();
    createCommandBuffers();
    createImageViews();

    spdlog::info("Created the swapchain");
}

Swapchain::~Swapchain() {
    // wait for device to be idle
    renderer.getDevice().waitIdle();

    // delete the command pool
    renderer.getDevice().destroyCommandPool(commandPool);

    // delete the image views
    for (auto imageView: swapchainImageViews) {
        renderer.getDevice().destroyImageView(imageView);
    }

    // delete the semaphores
    for (auto semaphore: imageAvailableSemaphores) {
        renderer.getDevice().destroySemaphore(semaphore);
    }

    for (auto semaphore: renderFinishedSemaphores) {
        renderer.getDevice().destroySemaphore(semaphore);
    }

    // delete the fences
    for (auto fence: inFlightFences) {
        renderer.getDevice().destroyFence(fence);
    }

    renderer.getDevice().destroySwapchainKHR(swapchain);

    // delete the surface
    renderer.getInstance().destroySurfaceKHR(surface);

    spdlog::info("Destroyed the swapchain");
}

void Swapchain::waitForFrame() const {
    const auto fence = inFlightFences[currentFrameInFlight];
    auto result = renderer.getDevice().waitForFences(1, &fence, VK_TRUE, UINT64_MAX);

    if (result != vk::Result::eSuccess) {
        spdlog::error("Failed to wait for fence");
    }
}

void Swapchain::waitForImageInFlight() {
    auto imageFence = imagesInFlight[currentImageIndex];
    auto inFlightFence = inFlightFences[currentFrameInFlight];

    if (imageFence != VK_NULL_HANDLE) {
        auto result = renderer.getDevice().waitForFences(1, &imageFence, VK_TRUE, UINT64_MAX);
        if (result != vk::Result::eSuccess) {
            spdlog::error("Failed to wait for image in flight fence");
        }
    }

    imagesInFlight[currentImageIndex] = inFlightFences[currentFrameInFlight];

    // reset the fence
    auto resetResult = renderer.getDevice().resetFences(1, &inFlightFence);

    if (resetResult != vk::Result::eSuccess) {
        spdlog::error("Failed to reset fence");
    }

  frameStats.update();
}

void Swapchain::acquireNextImage() {
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{};
    acquireNextImageInfo.setSwapchain(swapchain);
    acquireNextImageInfo.setTimeout(UINT64_MAX);
    acquireNextImageInfo.setSemaphore(imageAvailableSemaphores[currentFrameInFlight]);
    acquireNextImageInfo.setDeviceMask(1);
    ;

    auto result = renderer.getDevice().acquireNextImage2KHR(acquireNextImageInfo);

    currentImageIndex = result.value;
}

void Swapchain::presentImage() {
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(&swapchain);
    presentInfo.setPImageIndices(&currentImageIndex);
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(&renderFinishedSemaphores[currentFrameInFlight]);

    auto result = renderer.getGraphicsQueue().presentKHR(presentInfo);

    if (result != vk::Result::eSuccess) {
        spdlog::error("Failed to present image");
    }

    currentFrameInFlight = (currentFrameInFlight + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Swapchain::createSwapchain(Window &window) {
    surface = window.createSurface(renderer);

    // get the surface capabilities
    auto surfaceCapabilities = renderer.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    auto surfaceFormats = renderer.getPhysicalDevice().getSurfaceFormatsKHR(surface);
    auto presentModes = renderer.getPhysicalDevice().getSurfacePresentModesKHR(surface);

    vk::SurfaceFormatKHR surfaceFormat;

    // find the surface format with SRGB
    for (auto format: surfaceFormats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb) {
            surfaceFormat = format;
            break;
        }
    }

    // check for FIFO present mode
    auto presentMode = vk::PresentModeKHR::eFifo;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.setSurface(surface);
    swapchainCreateInfo.setMinImageCount(surfaceCapabilities.minImageCount + 1);
    swapchainCreateInfo.setImageFormat(surfaceFormat.format);
    swapchainCreateInfo.setImageColorSpace(surfaceFormat.colorSpace);
    swapchainCreateInfo.setImageExtent(surfaceCapabilities.currentExtent);
    swapchainCreateInfo.setImageArrayLayers(1);
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    swapchainCreateInfo.setQueueFamilyIndexCount(0);
    swapchainCreateInfo.setPQueueFamilyIndices(nullptr);
    swapchainCreateInfo.setPresentMode(presentMode);

    auto result = renderer.getDevice().createSwapchainKHR(swapchainCreateInfo);

    swapchain = result;

    // get the image count
    imageCount = renderer.getDevice().getSwapchainImagesKHR(swapchain).size();

    // set the extent
    swapchainExtent = surfaceCapabilities.currentExtent;
    swapchainFormat = surfaceFormat.format;
}

void Swapchain::createSemaphores() {
    vk::SemaphoreCreateInfo semaphoreCreateInfo{};

    // semaphores per swapchain image
    for (uint32_t i = 0; i < imageCount; i++) {
        imageAvailableSemaphores.push_back(renderer.getDevice().createSemaphore(semaphoreCreateInfo));
        renderFinishedSemaphores.push_back(renderer.getDevice().createSemaphore(semaphoreCreateInfo));
    }
}

void Swapchain::createFences() {
    // create fences for each frame in flight
    vk::FenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        const auto fence = renderer.getDevice().createFence(fenceCreateInfo);
        inFlightFences.push_back(fence);
    }

    imagesInFlight.resize(imageCount, VK_NULL_HANDLE);
}

void Swapchain::createSwapchainImages() { swapchainImages = renderer.getDevice().getSwapchainImagesKHR(swapchain); }

void Swapchain::createCommandBuffers() {
    // create the command pool
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.setQueueFamilyIndex(renderer.getGraphicsQueueFamilyIndex());
    commandPoolCreateInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    commandPool = renderer.getDevice().createCommandPool(commandPoolCreateInfo);

    if (!commandPool) {
        spdlog::error("Failed to create command pool");
    }

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.setCommandPool(commandPool);
    commandBufferAllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    commandBufferAllocateInfo.setCommandBufferCount(imageCount);

    commandBuffers = renderer.getDevice().allocateCommandBuffers(commandBufferAllocateInfo);
}

void Swapchain::submitCommandBuffer() {
    // get current wait and signal semaphores
    auto wait = imageAvailableSemaphores[currentFrameInFlight];
    auto signal = renderFinishedSemaphores[currentFrameInFlight];
    auto fence = inFlightFences[currentFrameInFlight];

    auto commandBuffer = commandBuffers[currentImageIndex];

    vk::SemaphoreSubmitInfo waitSemaphoreInfo{};
    waitSemaphoreInfo.setSemaphore(wait);
    waitSemaphoreInfo.setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::SemaphoreSubmitInfo signalSemaphoreInfo{};
    signalSemaphoreInfo.setSemaphore(signal);
    signalSemaphoreInfo.setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);

    vk::CommandBufferSubmitInfo commandBufferSubmitInfo{};
    commandBufferSubmitInfo.setCommandBuffer(commandBuffer);

    vk::SubmitInfo2 submitInfo{};
    submitInfo.setWaitSemaphoreInfos(waitSemaphoreInfo);
    submitInfo.setCommandBufferInfos(commandBufferSubmitInfo);
    submitInfo.setSignalSemaphoreInfos(signalSemaphoreInfo);

    renderer.getGraphicsQueue().submit2KHR(submitInfo, fence);
}

void Swapchain::createImageViews() {
    // for every image, create a swapchain image
    for (auto image: swapchainImages) {
        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.setImage(image);
        imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
        imageViewCreateInfo.setFormat(swapchainFormat);
        imageViewCreateInfo.setComponents(vk::ComponentMapping{});
        imageViewCreateInfo.setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        auto imageView = renderer.getDevice().createImageView(imageViewCreateInfo);

        swapchainImageViews.push_back(imageView);
    }
}


} // namespace braque
