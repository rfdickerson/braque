#include <spdlog/spdlog.h>
#include <braque/engine.h>
#include <braque/uniforms.h>
#include <braque/image_loader.h>

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

  const auto& swapchain = engine_.getSwapchain();

  for (uint32_t i = 0; i < swapchain.getFramesInFlightCount(); ++i) {
    camera_buffers_.push_back(Buffer(engine_, BufferType::uniform, sizeof(CameraUbo)));
  }

  spdlog::info("Create uniforms");
}

void Uniforms::createDescriptorSetLayout() {
  const auto& device = engine_.getRenderer().getDevice();

  vk::DescriptorSetLayoutBinding cameraBinding{};
  cameraBinding.setBinding(CAMERA_BINDING);
  cameraBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
  cameraBinding.setDescriptorCount(1);
  cameraBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);

  vk::DescriptorSetLayoutBinding samplerBinding{};
  samplerBinding.binding = 1;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
  samplerBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

  std::array bindings = {cameraBinding, samplerBinding};

  vk::DescriptorSetLayoutCreateInfo layoutInfo;
  layoutInfo.setBindings(bindings);

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

void Uniforms::SetTextureData( const Texture& texture, vk::Sampler sampler) {

  vk::DescriptorImageInfo imageInfo {};

  imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
  imageInfo.setImageView(texture.GetImageView());
  imageInfo.setSampler(sampler);

  // for each swapchain descriptor
  for (uint32_t i = 0; i < Swapchain::getFramesInFlightCount(); ++i) {

    vk::WriteDescriptorSet descriptorWrite;

    descriptorWrite.setDstSet(descriptor_sets_[i]);
    descriptorWrite.setDstBinding(1);
    descriptorWrite.setDstArrayElement(0);
    descriptorWrite.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    descriptorWrite.setDescriptorCount(1);
    descriptorWrite.setImageInfo(imageInfo);

    engine_.getRenderer().getDevice().updateDescriptorSets(descriptorWrite, nullptr);
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