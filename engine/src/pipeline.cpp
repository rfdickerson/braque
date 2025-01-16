//
// Created by Robert F. Dickerson on 12/20/24.
//

#include "braque/pipeline.h"
#include "braque/scene.h"
#include "braque/shader.h"

#include <spdlog/spdlog.h>

namespace braque {

Pipeline::Pipeline(vk::Device device, Shader& shader,
                   vk::DescriptorSetLayout descriptor_set_layout)
    : device(device) {

  constexpr uint32_t width = 800;
  constexpr uint32_t height = 600;

  vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.setSetLayouts(descriptor_set_layout);

  layout_ = device.createPipelineLayout(pipelineLayoutInfo);

  auto shaderStages = shader.getPipelineShaderStageCreateInfos();

  vk::VertexInputBindingDescription bindingDescription{};
  bindingDescription.setBinding(0);
  bindingDescription.setStride(sizeof(Vertex));
  bindingDescription.setInputRate(vk::VertexInputRate::eVertex);

  std::array<vk::VertexInputAttributeDescription, 4> attributeDescription{};
  attributeDescription[0].setBinding(0);
  attributeDescription[0].setLocation(0);
  attributeDescription[0].setFormat(vk::Format::eR32G32B32Sfloat);
  attributeDescription[0].setOffset(0);

  attributeDescription[1].setBinding(0);
  attributeDescription[1].setLocation(1);
  attributeDescription[1].setFormat(vk::Format::eR32G32B32Sfloat);
  attributeDescription[1].setOffset(sizeof(float) * 3);

  attributeDescription[2].setBinding(0);
  attributeDescription[2].setLocation(2);
  attributeDescription[2].setFormat(vk::Format::eR32G32B32Sfloat);
  attributeDescription[2].setOffset(sizeof(float) * 6);

  attributeDescription[3].setBinding(0);
  attributeDescription[3].setLocation(3);
  attributeDescription[3].setFormat(vk::Format::eR32G32Sfloat);
  attributeDescription[3].setOffset(sizeof(float) * 9);

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.setVertexBindingDescriptions(bindingDescription);
  vertexInputInfo.setVertexAttributeDescriptions(attributeDescription);

  vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
  inputAssembly.setPrimitiveRestartEnable(vk::False);

  vk::Viewport viewport{0, 0, width, height, 0, 1};
  vk::Rect2D scissor{{0, 0}, {width, height}};

  vk::PipelineViewportStateCreateInfo viewportState{};
  viewportState.setViewports(viewport);
  viewportState.setScissors(scissor);

  vk::PipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.setDepthClampEnable(vk::False);
  rasterizer.setRasterizerDiscardEnable(vk::False);
  rasterizer.setPolygonMode(vk::PolygonMode::eFill);
  rasterizer.setLineWidth(1.0F);
  rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
  rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);
  rasterizer.setDepthBiasEnable(vk::False);

  vk::PipelineMultisampleStateCreateInfo multisampling{};
  multisampling.setSampleShadingEnable(vk::False);
  multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e4);

  vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.setColorWriteMask(
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
  colorBlendAttachment.setBlendEnable(vk::False);

  vk::PipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.setLogicOpEnable(vk::False);
  colorBlending.setLogicOp(vk::LogicOp::eCopy);
  colorBlending.setAttachments(colorBlendAttachment);
  colorBlending.setBlendConstants({0, 0, 0, 0});

  auto dynamicStates =
      std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};

  vk::PipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.setDynamicStates(dynamicStates);

  vk::PipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.setDepthTestEnable(vk::True);
  depthStencil.setDepthWriteEnable(vk::True);
  depthStencil.setDepthCompareOp(vk::CompareOp::eGreater);
  depthStencil.setDepthBoundsTestEnable(vk::False);
  depthStencil.setStencilTestEnable(vk::False);
  depthStencil.setMinDepthBounds(0.0F);
  depthStencil.setMaxDepthBounds(1.0F);


  auto colorFormat = {vk::Format::eR16G16B16A16Sfloat};
  constexpr auto depthFormat = vk::Format::eD32Sfloat;

  vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo {};
  pipelineRenderingCreateInfo.setColorAttachmentFormats(colorFormat);
  pipelineRenderingCreateInfo.setDepthAttachmentFormat(depthFormat);

  vk::GraphicsPipelineCreateInfo pipelineInfo{};

  pipelineInfo.setStages(shaderStages);
  pipelineInfo.setPVertexInputState(&vertexInputInfo);
  pipelineInfo.setPInputAssemblyState(&inputAssembly);
  pipelineInfo.setPViewportState(&viewportState);
  pipelineInfo.setPRasterizationState(&rasterizer);
  pipelineInfo.setPMultisampleState(&multisampling);
  pipelineInfo.setPColorBlendState(&colorBlending);
  pipelineInfo.setPDynamicState(&dynamicState);
  pipelineInfo.setLayout(layout_);
  pipelineInfo.setRenderPass(nullptr);
  pipelineInfo.setSubpass(0);
  pipelineInfo.setPNext(&pipelineRenderingCreateInfo);
  pipelineInfo.setPDepthStencilState(&depthStencil);

  auto result = device.createGraphicsPipeline(nullptr, pipelineInfo);

  if (result.result != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to create graphics pipeline");
  }

  pipeline = result.value;

  spdlog::info("Successfully created pipeline");
}

Pipeline::~Pipeline() {
  device.destroyPipeline(pipeline);
  device.destroyPipelineLayout(layout_);
}

void Pipeline::Bind(vk::CommandBuffer buffer) {
  buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
}

void Pipeline::SetViewport(const vk::CommandBuffer buffer,
                           const vk::Viewport& viewport) {
  buffer.setViewport(0, viewport);
}

void Pipeline::SetScissor(const vk::CommandBuffer buffer,
                          const vk::Rect2D scissor) {
  buffer.setScissor(0, scissor);
}

void Pipeline::Draw(const vk::CommandBuffer buffer) {
  buffer.draw(3, 1, 0, 0);
}

vk::PipelineLayout Pipeline::VulkanLayout() const {
  return layout_;
}

}  // namespace braque