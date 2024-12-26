//
// Created by Robert F. Dickerson on 12/16/24.
//

#include "braque/memory_allocator.hpp"

#define VMA_IMPLEMENTATION
#include "braque/renderer.hpp"

#include <spdlog/spdlog.h>
#include <vk_mem_alloc.h>

namespace braque
{

  MemoryAllocator::MemoryAllocator( const Renderer & renderer ) : allocator( VK_NULL_HANDLE )
  {
    VmaVulkanFunctions vulkanFunctions{};
    vulkanFunctions.vkGetInstanceProcAddr         = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vulkanFunctions.vkGetPhysicalDeviceProperties = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties;
    vulkanFunctions.vkGetDeviceProcAddr           = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice   = renderer.getPhysicalDevice();
    allocatorInfo.device           = renderer.getDevice();
    allocatorInfo.instance         = renderer.getInstance();
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.pVulkanFunctions = &vulkanFunctions;
    allocatorInfo.flags            = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;

    if ( const auto result = vmaCreateAllocator( &allocatorInfo, &allocator ); result != VK_SUCCESS )
    {
      spdlog::error( "Failed to create memory allocator" );
      throw std::runtime_error( "Failed to create memory allocator" );
    }

    spdlog::info( "Created memory allocator" );

    // get some stats from the allocator
    VmaTotalStatistics stats{};
    vmaCalculateStatistics( allocator, &stats );
    spdlog::info( "Memory allocator stats: {}", stats.total.statistics.allocationCount );
  }

  MemoryAllocator::~MemoryAllocator()
  {
    vmaDestroyAllocator( allocator );
  }

  auto MemoryAllocator::createImage( const vk::ImageCreateInfo & createInfo, const VmaAllocationCreateInfo & allocInfo ) const -> AllocatedImage
  {
    AllocatedImage allocatedImage{};

    VkImage                 image        = nullptr;
    VmaAllocation           allocation   = nullptr;
    const VkImageCreateInfo vkCreateInfo = createInfo;

    const auto result = vmaCreateImage( allocator, &vkCreateInfo, &allocInfo, &image, &allocation, nullptr );
    if ( result != VK_SUCCESS )
    {
      spdlog::error( "Failed to create image" );
      throw std::runtime_error( "Failed to create image" );
    }

    allocatedImage.image      = image;
    allocatedImage.allocation = allocation;

    return allocatedImage;
  }

  void MemoryAllocator::destroyImage( const AllocatedImage & image ) const
  {
    vmaDestroyImage( allocator, image.image, image.allocation );
  }

  void MemoryAllocator::destroyBuffer( const braque::AllocatedBuffer & buffer ) const
  {

    if ( buffer.mappedData )
    {
      vmaUnmapMemory( allocator, buffer.allocation );
    }

    vmaDestroyBuffer( allocator, buffer.buffer, buffer.allocation );
  }

  auto MemoryAllocator::getReport() const -> MemoryReport
  {
    MemoryReport                               report{};
    std::array<VmaBudget, VK_MAX_MEMORY_HEAPS> budgets{};

    vmaGetHeapBudgets( allocator, budgets.data() );
    report.allocations = budgets[0].statistics.allocationCount;
    report.totalMemory = budgets[0].statistics.allocationBytes;
    report.usedMemory  = budgets[0].usage;
    return report;
  }

  AllocatedBuffer MemoryAllocator::createBuffer( const vk::BufferCreateInfo & createInfo, const VmaAllocationCreateInfo & allocInfo ) const
  {
    AllocatedBuffer allocatedBuffer{};

    VkBuffer                 buffer       = nullptr;
    VmaAllocation            allocation   = nullptr;
    const VkBufferCreateInfo vkCreateInfo = createInfo;

    const auto result = vmaCreateBuffer( allocator, &vkCreateInfo, &allocInfo, &buffer, &allocation, nullptr );
    if ( result != VK_SUCCESS )
    {
      spdlog::error( "Failed to create buffer" );
      throw std::runtime_error( "Failed to create buffer" );
    }

    allocatedBuffer.buffer     = buffer;
    allocatedBuffer.allocation = allocation;

    // map the buffer
    vmaMapMemory( allocator, allocation, &allocatedBuffer.mappedData );

    return allocatedBuffer;
  }

    void MemoryAllocator::WriteData( vk::CommandBuffer buffer, const AllocatedBuffer & bufferInfo, const void * data, size_t size ) const
    {
        std::memcpy( bufferInfo.mappedData, data, size );
        auto result = vmaFlushAllocation(allocator, bufferInfo.allocation, 0, VK_WHOLE_SIZE);

        if (result != VK_SUCCESS)
        {
            spdlog::error("Failed to flush memory");
        }

      vk::BufferMemoryBarrier barrier;
        barrier.setBuffer(bufferInfo.buffer);
        barrier.setSize(VK_WHOLE_SIZE);
        barrier.setSrcAccessMask(vk::AccessFlagBits::eHostWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eUniformRead);
        barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored);
        barrier.setDstQueueFamilyIndex(vk::QueueFamilyIgnored);

        buffer.pipelineBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eVertexInput, vk::DependencyFlagBits::eByRegion, nullptr, barrier, nullptr);
    }

}  // namespace braque
