//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "swapchain.hpp"

#include <spdlog/spdlog.h>

namespace braque {

Swapchain::Swapchain(Window& window, Renderer& renderer): renderer(renderer) {

    surface = window.createSurface(renderer);

    // get the surface capabilities
    auto surfaceCapabilities = renderer.getPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    auto surfaceFormats = renderer.getPhysicalDevice().getSurfaceFormatsKHR(surface);
    auto presentModes = renderer.getPhysicalDevice().getSurfacePresentModesKHR(surface);

    // check for FIFO present mode
    auto presentMode = vk::PresentModeKHR::eFifo;

    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.setSurface(surface);
    swapchainCreateInfo.setMinImageCount(surfaceCapabilities.minImageCount);
    swapchainCreateInfo.setImageFormat(surfaceFormats[0].format);
    swapchainCreateInfo.setImageColorSpace(surfaceFormats[0].colorSpace);
    swapchainCreateInfo.setImageExtent(surfaceCapabilities.currentExtent);
    swapchainCreateInfo.setImageArrayLayers(1);
    swapchainCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    swapchainCreateInfo.setQueueFamilyIndexCount(0);
    swapchainCreateInfo.setPQueueFamilyIndices(nullptr);

    auto result = renderer.getDevice().createSwapchainKHR(swapchainCreateInfo);

    swapchain = result;

}

Swapchain::~Swapchain() {

    renderer.getDevice().destroySwapchainKHR(swapchain);

    // delete the surface
    renderer.getInstance().destroySurfaceKHR(surface);

    spdlog::info("Destroyed the swapchain");
}

} // braque