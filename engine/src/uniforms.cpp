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
    createUniformBuffers();
    createDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
  }

  Uniforms::~Uniforms()
  {
    const auto & allocator = engine_.getMemoryAllocator();

    // Destroy the descriptor pool
    const auto & device = engine_.getRenderer().getDevice();

    device.destroyDescriptorSetLayout( descriptorSetLayout );
    device.destroyDescriptorPool( descriptorPool );

    for ( auto & buffer : cameraBuffers )
    {
      allocator.destroyBuffer( buffer );
    }
  }

  void Uniforms::createUniformBuffers()
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

    for ( uint32_t i = 0; i < swapchain.getFramesInFlightCount(); ++i )
    {
      cameraBuffers.push_back( allocator.createBuffer( cameraBufferCreateInfo, cameraBufferAllocInfo ) );
    }

    spdlog::info( "Create uniforms" );
  }

  void Uniforms::createDescriptorSetLayout()
  {
    const auto & device = engine_.getRenderer().getDevice();

    vk::DescriptorSetLayoutBinding cameraBinding;
    cameraBinding.setBinding( CAMERA_BINDING );
    cameraBinding.setDescriptorType( vk::DescriptorType::eUniformBuffer );
    cameraBinding.setDescriptorCount( 1 );
    cameraBinding.setStageFlags( vk::ShaderStageFlagBits::eVertex );

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.setBindings( cameraBinding );

    descriptorSetLayout = device.createDescriptorSetLayout( layoutInfo );
  }

  void Uniforms::createDescriptorPool()
  {
    const auto & device = engine_.getRenderer().getDevice();

    vk::DescriptorPoolSize poolSize;
    poolSize.setType( vk::DescriptorType::eUniformBuffer );
    poolSize.setDescriptorCount( static_cast<uint32_t>( cameraBuffers.size() ) );

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.setPoolSizes( poolSize );
    poolInfo.setMaxSets( static_cast<uint32_t>( cameraBuffers.size() ) );

    descriptorPool = device.createDescriptorPool( poolInfo );
  }

  void Uniforms::createDescriptorSets()
  {
    const auto & device = engine_.getRenderer().getDevice();

    std::vector<vk::DescriptorSetLayout> layouts( cameraBuffers.size(), descriptorSetLayout );

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool( descriptorPool );
    allocInfo.setSetLayouts( layouts );

    descriptorSets = device.allocateDescriptorSets( allocInfo );

    for ( size_t i = 0; i < cameraBuffers.size(); ++i )
    {
      vk::DescriptorBufferInfo bufferInfo;
      bufferInfo.setBuffer( cameraBuffers[i].buffer );
      bufferInfo.setOffset( 0 );
      bufferInfo.setRange( sizeof( CameraUbo ) );

      vk::WriteDescriptorSet descriptorWrite;
      descriptorWrite.setDstSet( descriptorSets[i] );
      descriptorWrite.setDstBinding( CAMERA_BINDING );
      descriptorWrite.setDstArrayElement( 0 );
      descriptorWrite.setDescriptorType( vk::DescriptorType::eUniformBuffer );
      descriptorWrite.setDescriptorCount( 1 );
      descriptorWrite.setPBufferInfo( &bufferInfo );

      device.updateDescriptorSets( descriptorWrite, nullptr );
    }
  }

}  // namespace braque