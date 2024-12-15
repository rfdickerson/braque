//
// Created by Robert F. Dickerson on 12/15/24.
//

#include "rendering_stage.hpp"

#include "renderer.hpp"
#include "swapchain.hpp"

#include <spdlog/spdlog.h>

namespace braque {

RenderingStage::RenderingStage(Renderer &renderer, Swapchain &swapchain): renderer(renderer), swapchain(swapchain) {
    spdlog::info("Creating rendering stage");
}

RenderingStage::~RenderingStage() {
    spdlog::info("Destroying rendering stage");
}

void RenderingStage::render() {
    auto buffer = swapchain.getCommandBuffer();
    buffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Define the image memory barrier using synchronization2
    vk::ImageMemoryBarrier2 imageBarrier{
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,          // srcStageMask
        vk::AccessFlagBits2::eColorAttachmentWrite,                      // srcAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,       // dstStageMask
        vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
        vk::ImageLayout::eUndefined,                     // oldLayout
        vk::ImageLayout::ePresentSrcKHR,                 // newLayout
        VK_QUEUE_FAMILY_IGNORED,                         // srcQueueFamilyIndex
        VK_QUEUE_FAMILY_IGNORED,                         // dstQueueFamilyIndex
        swapchain.getSwapchainImage(),                   // image
        {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}    // subresourceRange
    };

    // Encapsulate the barrier in a dependency info object
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setImageMemoryBarrierCount(1);
    dependencyInfo.setPImageMemoryBarriers(&imageBarrier);

    // Issue the pipeline barrier with synchronization2
    buffer.pipelineBarrier2KHR(dependencyInfo);

    // End recording
    buffer.end();
}

} // braque