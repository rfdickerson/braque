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

    createDescriptorPool();
}

RenderingStage::~RenderingStage() {

    // destroy the descriptor pool
    renderer.getDevice().destroyDescriptorPool(descriptorPool);

    spdlog::info("Destroying rendering stage");
}

void RenderingStage::begin(vk::CommandBuffer buffer) {
    buffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
}

void RenderingStage::beginRenderingPass(vk::CommandBuffer buffer) {

    vk::ClearColorValue clearColor;
    clearColor.setFloat32({1.0f, 0.0f, 0.0f, 1.0f});

     vk::RenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.setClearValue(clearColor);
    renderingAttachmentInfo.setLoadOp(vk::AttachmentLoadOp::eClear);
    renderingAttachmentInfo.setStoreOp(vk::AttachmentStoreOp::eStore);
    renderingAttachmentInfo.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
    renderingAttachmentInfo.setImageView(swapchain.getImageView());

    vk::RenderingInfo renderingInfo{};
    renderingInfo.setColorAttachmentCount(1);
    renderingInfo.setPColorAttachments(&renderingAttachmentInfo);
    renderingInfo.setLayerCount(1);
    renderingInfo.setRenderArea(swapchain.getExtent());

    buffer.beginRenderingKHR(renderingInfo);
}

void RenderingStage::endRenderingPass(vk::CommandBuffer buffer) {
    buffer.endRenderingKHR();
}

void RenderingStage::end(vk::CommandBuffer buffer) {
    buffer.end();
}

void RenderingStage::prepareImageForColorAttachment(vk::CommandBuffer buffer) {

    // Define the image memory barrier using synchronization2
    vk::ImageMemoryBarrier2 imageBarrier{
        vk::PipelineStageFlagBits2::eTopOfPipe,          // srcStageMask
        vk::AccessFlagBits2::eNone,                      // srcAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,       // dstStageMask
        vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
        vk::ImageLayout::eUndefined,                     // oldLayout
        vk::ImageLayout::eColorAttachmentOptimal,        // newLayout
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

}

void RenderingStage::prepareImageForDisplay(vk::CommandBuffer buffer) {

    // Define the image memory barrier using synchronization2
    vk::ImageMemoryBarrier2 imageBarrier{
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,          // srcStageMask
        vk::AccessFlagBits2::eColorAttachmentWrite,                      // srcAccessMask
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,       // dstStageMask
        vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
        vk::ImageLayout::eColorAttachmentOptimal,                     // oldLayout
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

}

void RenderingStage::createDescriptorPool() {
    constexpr auto descriptorCount = 1000;

    constexpr std::array<vk::DescriptorPoolSize, 11> poolSizes = {
        vk::DescriptorPoolSize{vk::DescriptorType::eSampler, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, descriptorCount},
        vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, descriptorCount}
    };

    const vk::DescriptorPoolCreateInfo poolInfo = vk::DescriptorPoolCreateInfo{}
    .setPoolSizeCount(poolSizes.size())
    .setPPoolSizes(poolSizes.data())
    .setMaxSets(descriptorCount)
    .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    descriptorPool = renderer.getDevice().createDescriptorPool(poolInfo, nullptr);

}


} // braque