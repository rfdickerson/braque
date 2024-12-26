//
// Created by Robert F. Dickerson on 12/20/24.
//

#include "braque/pipeline.hpp"

#include <spdlog/spdlog.h>

namespace braque
{

  Pipeline::Pipeline( vk::Device device, Shader & shader, vk::DescriptorSetLayout descriptor_set_layout ) : device( device )
  {

    constexpr uint32_t width = 800;
    constexpr uint32_t height = 600;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayouts(descriptor_set_layout);

    layout_ = device.createPipelineLayout( pipelineLayoutInfo );

    auto shaderStages = shader.getPipelineShaderStageCreateInfos();

    constexpr vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vk::PipelineInputAssemblyStateCreateInfo     inputAssembly{};
    inputAssembly.setTopology( vk::PrimitiveTopology::eTriangleList );
    inputAssembly.setPrimitiveRestartEnable( vk::False );

    vk::Viewport viewport{ 0, 0, width, height, 0, 1 };
    vk::Rect2D   scissor{ { 0, 0 }, { width, height } };

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewports( viewport );
    viewportState.setScissors( scissor );

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable( vk::False );
    rasterizer.setRasterizerDiscardEnable( vk::False );
    rasterizer.setPolygonMode( vk::PolygonMode::eFill );
    rasterizer.setLineWidth( 1.0F );
    rasterizer.setCullMode( vk::CullModeFlagBits::eBack );
    rasterizer.setFrontFace( vk::FrontFace::eClockwise );
    rasterizer.setDepthBiasEnable( vk::False );

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setSampleShadingEnable( vk::False );
    multisampling.setRasterizationSamples( vk::SampleCountFlagBits::e1 );

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setColorWriteMask( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
                                            vk::ColorComponentFlagBits::eA );
    colorBlendAttachment.setBlendEnable( vk::False );

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.setLogicOpEnable( vk::False );
    colorBlending.setLogicOp( vk::LogicOp::eCopy );
    colorBlending.setAttachments( colorBlendAttachment );
    colorBlending.setBlendConstants( { 0, 0, 0, 0 } );

    auto dynamicStates = std::array{ vk::DynamicState::eViewport, vk::DynamicState::eScissor };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStates( dynamicStates );

    constexpr auto                      colorFormat = vk::Format::eB8G8R8A8Srgb;
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
    pipelineRenderingCreateInfo.setColorAttachmentCount( 1 );
    pipelineRenderingCreateInfo.setPColorAttachmentFormats( &colorFormat );

    vk::GraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.setStages( shaderStages );
    pipelineInfo.setPVertexInputState( &vertexInputInfo );
    pipelineInfo.setPInputAssemblyState( &inputAssembly );
    pipelineInfo.setPViewportState( &viewportState );
    pipelineInfo.setPRasterizationState( &rasterizer );
    pipelineInfo.setPMultisampleState( &multisampling );
    pipelineInfo.setPColorBlendState( &colorBlending );
    pipelineInfo.setPDynamicState( &dynamicState );
    pipelineInfo.setLayout( layout_ );
    pipelineInfo.setRenderPass( nullptr );
    pipelineInfo.setSubpass( 0 );
    pipelineInfo.setPNext( &pipelineRenderingCreateInfo );

    auto result = device.createGraphicsPipeline( nullptr, pipelineInfo );

    if ( result.result != vk::Result::eSuccess )
    {
      throw std::runtime_error( "Failed to create graphics pipeline" );
    }

    pipeline = result.value;

    spdlog::info( "Successfully created pipeline" );
  }

  Pipeline::~Pipeline()
  {
    device.destroyPipeline( pipeline );
    device.destroyPipelineLayout( layout_ );
  }

  void Pipeline::Bind( vk::CommandBuffer buffer )
  {
    buffer.bindPipeline( vk::PipelineBindPoint::eGraphics, pipeline );
  }

  void Pipeline::SetViewport( const vk::CommandBuffer buffer, const vk::Viewport & viewport )
  {
    buffer.setViewport( 0, viewport );
  }

  void Pipeline::SetScissor( const vk::CommandBuffer buffer, const vk::Rect2D scissor )
  {
    buffer.setScissor( 0, scissor );
  }

  void Pipeline::Draw( const vk::CommandBuffer buffer )
  {
    buffer.draw( 3, 1, 0, 0 );
  }

  vk::PipelineLayout Pipeline::VulkanLayout() const {
    return layout_;
  }


}  // namespace braque