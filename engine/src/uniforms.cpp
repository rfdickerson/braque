#include <braque/engine.hpp>
#include <braque/uniforms.hpp>

#include <spdlog/spdlog.h>

namespace braque
{

  constexpr uint32_t CAMERA_BINDING = 0;

  struct CameraUbo
  {
    glm::mat4 view;
    glm::mat4 proj;
  };

  Uniforms::Uniforms( Engine & engine ) : engine_( engine )
  {
    auto cameraBufferCreateInfo = vk::BufferCreateInfo{};
    cameraBufferCreateInfo.setSize( sizeof( CameraUbo ) );
    cameraBufferCreateInfo.setUsage( vk::BufferUsageFlagBits::eUniformBuffer );
    cameraBufferCreateInfo.setSharingMode( vk::SharingMode::eExclusive );

    auto cameraBufferAllocInfo  = VmaAllocationCreateInfo{};
    cameraBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    cameraBufferAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    const auto & swapchain = engine_.getSwapchain();
    const auto & allocator = engine_.getMemoryAllocator();

    for ( uint32_t i = 0; i < swapchain.getImageCount(); ++i )
    {
      cameraBuffers.push_back( allocator.createBuffer( cameraBufferCreateInfo, cameraBufferAllocInfo ) );
    }

    spdlog::info("Create uniforms");
  }

  Uniforms::~Uniforms()
  {
    const auto & allocator = engine_.getMemoryAllocator();

    for ( auto & buffer : cameraBuffers )
    {
      allocator.destroyBuffer( buffer );
    }
  }

}  // namespace braque