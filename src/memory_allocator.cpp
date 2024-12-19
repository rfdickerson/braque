//
// Created by Robert F. Dickerson on 12/16/24.
//

#include "memory_allocator.hpp"

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "renderer.hpp"

#include <spdlog/spdlog.h>

namespace braque {

MemoryAllocator::MemoryAllocator(Renderer &renderer) {
    VmaVulkanFunctions vulkanFunctions{};
    vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetPhysicalDeviceProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties;
    vulkanFunctions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice = renderer.getPhysicalDevice();
    allocatorInfo.device = renderer.getDevice();
    allocatorInfo.instance = renderer.getInstance();
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.pVulkanFunctions = &vulkanFunctions;
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | 	VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;

    auto result = vmaCreateAllocator(&allocatorInfo, &allocator);
    if (result != VK_SUCCESS) {
        spdlog::error("Failed to create memory allocator");
        throw std::runtime_error("Failed to create memory allocator");
    }

    spdlog::info("Created memory allocator");

    // get some stats from the allocator
    VmaTotalStatistics stats{};
    vmaCalculateStatistics(allocator, &stats);
    spdlog::info("Memory allocator stats: {}", stats.total.statistics.allocationCount);
}

MemoryAllocator::~MemoryAllocator() {
    vmaDestroyAllocator(allocator);
}

    AllocatedImage MemoryAllocator::createImage(const vk::ImageCreateInfo &createInfo, const VmaAllocationCreateInfo &allocInfo) {
        AllocatedImage allocatedImage{};

        vk::Image image;
        VmaAllocation allocation;

        auto result = vmaCreateImage(allocator, reinterpret_cast<const VkImageCreateInfo*>(&createInfo), &allocInfo, reinterpret_cast<VkImage*>(&image), &allocation, nullptr);
        if (result != VK_SUCCESS) {
            spdlog::error("Failed to create image");
            throw std::runtime_error("Failed to create image");
        }

        allocatedImage.image = image;
        allocatedImage.allocation = allocation;

        return allocatedImage;
    }

    void MemoryAllocator::destroyImage(AllocatedImage &image) {
        vmaDestroyImage(allocator, image.image, image.allocation);
    }

    MemoryReport MemoryAllocator::getReport() {
        MemoryReport report{};
        VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
        vmaGetHeapBudgets(allocator, budgets);
        report.allocations = budgets[0].statistics.allocationCount;
        report.totalMemory = budgets[0].statistics.allocationBytes;
        report.usedMemory = budgets[0].usage;
        return report;
    }

} // braque