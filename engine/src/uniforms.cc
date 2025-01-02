#include <spdlog/spdlog.h>
#include <braque/engine.h>
#include <braque/uniforms.h>

namespace braque {

constexpr uint32_t CAMERA_BINDING = 0;

struct CameraUbo {
  glm::mat4 view;
  glm::mat4 proj;
};

Uniforms::Uniforms(Engine& engine) : engine_(engine) {
  CreateUniformBuffers();
  createDescriptorSetLayout();
  createDescriptorPool();
  createDescriptorSets();
}

Uniforms::~Uniforms() {

  // Destroy the descriptor pool
  const auto& device = engine_.getRenderer().getDevice();

  device.destroyDescriptorSetLayout(descriptor_set_layout_);
  device.destroyDescriptorPool(descriptor_pool_);

}

void Uniforms::CreateUniformBuffers() {
  // auto cameraBufferCreateInfo = vk::BufferCreateInfo{};
  // cameraBufferCreateInfo.setSize(sizeof(CameraUbo));
  // cameraBufferCreateInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst);
  // cameraBufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
  //
  // auto cameraBufferAllocInfo = VmaAllocationCreateInfo{};
  // cameraBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  // cameraBufferAllocInfo.flags =
  //     VMA_ALLOCATION_CREATE_MAPPED_BIT |
  //     VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

  const auto& swapchain = engine_.getSwapchain();

  for (uint32_t i = 0; i < swapchain.getFramesInFlightCount(); ++i) {
    camera_buffers_.push_back(Buffer(engine_, BufferType::uniform, sizeof(CameraUbo)));
  }

  spdlog::info("Create uniforms");
}

void Uniforms::createDescriptorSetLayout() {
  const auto& device = engine_.getRenderer().getDevice();

  vk::DescriptorSetLayoutBinding cameraBinding;
  cameraBinding.setBinding(CAMERA_BINDING);
  cameraBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
  cameraBinding.setDescriptorCount(1);
  cameraBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);

  vk::DescriptorSetLayoutCreateInfo layoutInfo;
  layoutInfo.setBindings(cameraBinding);

  descriptor_set_layout_ = device.createDescriptorSetLayout(layoutInfo);
}

void Uniforms::createDescriptorPool() {
  const auto& device = engine_.getRenderer().getDevice();

  vk::DescriptorPoolSize poolSize;
  poolSize.setType(vk::DescriptorType::eUniformBuffer);
  poolSize.setDescriptorCount(static_cast<uint32_t>(camera_buffers_.size()));

  vk::DescriptorPoolCreateInfo poolInfo;
  poolInfo.setPoolSizes(poolSize);
  poolInfo.setMaxSets(static_cast<uint32_t>(camera_buffers_.size()));

  descriptor_pool_ = device.createDescriptorPool(poolInfo);
}

void Uniforms::createDescriptorSets() {
  const auto& device = engine_.getRenderer().getDevice();

  std::vector layouts(camera_buffers_.size(), descriptor_set_layout_);

  vk::DescriptorSetAllocateInfo allocInfo;
  allocInfo.setDescriptorPool(descriptor_pool_);
  allocInfo.setSetLayouts(layouts);

  descriptor_sets_ = device.allocateDescriptorSets(allocInfo);

  for (size_t i = 0; i < camera_buffers_.size(); ++i) {
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo.setBuffer(camera_buffers_[i].GetBuffer());
    bufferInfo.setOffset(0);
    bufferInfo.setRange(sizeof(CameraUbo));

    vk::WriteDescriptorSet descriptorWrite;
    descriptorWrite.setDstSet(descriptor_sets_[i]);
    descriptorWrite.setDstBinding(CAMERA_BINDING);
    descriptorWrite.setDstArrayElement(0);
    descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    descriptorWrite.setDescriptorCount(1);
    descriptorWrite.setPBufferInfo(&bufferInfo);

    device.updateDescriptorSets(descriptorWrite, nullptr);
  }
}

void Uniforms::SetCameraData(vk::CommandBuffer buffer, const Camera& camera) {

  const auto& frame = engine_.getSwapchain().CurrentFrameIndex();

  auto& cameraBuffer = camera_buffers_[frame];

  //auto& cameraUbo = *static_cast<CameraUbo*>(cameraBuffer.);
  //auto camera_ubo = cameraBuffer.GetPointer<CameraUbo>();
  CameraUbo camera_ubo;

  camera_ubo.view = camera.ViewMatrix();
  camera_ubo.proj = camera.ProjectionMatrix();

  //std::memcpy(cameraBuffer.mappedData, &cameraUbo, sizeof(CameraUbo));

  //   const auto& allocator = engine_.getMemoryAllocator();
  // allocator.WriteData(buffer, cameraBuffer, &camera_ubo, sizeof(CameraUbo));

  cameraBuffer.CopyData(buffer, &camera_ubo, sizeof(CameraUbo));
}

void Uniforms::Bind(vk::CommandBuffer buffer) const {
  const auto& frame = engine_.getSwapchain().CurrentFrameIndex();

  const auto layout = engine_.getRenderingStage().GetPipeline().VulkanLayout();

  buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                            layout, 0,
                            descriptor_sets_[frame], nullptr);
}

}  // namespace braque