#include <braque/engine.h>
#include <braque/texture.h>
#include <braque/uniforms.h>
#include <spdlog/spdlog.h>

namespace braque {

constexpr uint32_t CAMERA_BINDING = 0;

struct CameraUbo {
  glm::mat4 view;
  glm::mat4 proj;
};

Uniforms::Uniforms(EngineContext& engine, Swapchain& swapchain) : engine_(engine), swapchain_(swapchain) {
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

  for (uint32_t i = 0; i < Swapchain::getFramesInFlightCount(); ++i) {
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
  cameraBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

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

  std::array<vk::DescriptorPoolSize, 2> poolSizes{};

  // for uniform buffer
  poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
  poolSizes[0].setDescriptorCount(static_cast<uint32_t>(camera_buffers_.size()));

  // for combined image sampler
  poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
  poolSizes[1].setDescriptorCount(static_cast<uint32_t>(camera_buffers_.size()));

  vk::DescriptorPoolCreateInfo poolInfo;
  poolInfo.setPoolSizes(poolSizes);
  poolInfo.setMaxSets(10);

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

  const auto& frame = swapchain_.CurrentFrameIndex();

  auto& cameraBuffer = camera_buffers_[frame];

  CameraUbo camera_ubo;

  camera_ubo.view = camera.ViewMatrix();
  camera_ubo.proj = camera.ProjectionMatrix();

  cameraBuffer.CopyData(buffer, &camera_ubo, sizeof(CameraUbo));
}

void Uniforms::Bind(vk::CommandBuffer buffer, vk::PipelineLayout layout) const {
  const auto& frame = swapchain_.CurrentFrameIndex();

  buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                            layout, 0,
                            descriptor_sets_[frame], nullptr);
}

}  // namespace braque