//
// Created by Robert F. Dickerson on 12/19/24.
//
#include "braque/shader.h"

#include <fstream>
#include <spdlog/spdlog.h>

namespace braque
{

  // read file helper function
  std::vector<uint8_t> ReadFile( const std::string & filename )
  {
    // Open the file in binary mode
    std::ifstream file( filename, std::ios::ate | std::ios::binary );

    if ( !file.is_open() )
    {
      throw std::runtime_error( "Failed to open file!" );
    }

    // Get the size of the file and allocate a buffer
    size_t            fileSize = (size_t)file.tellg();
    std::vector<uint8_t> buffer( fileSize );

    // Read the file into the buffer
    file.seekg( 0 );
    file.read( reinterpret_cast<std::istream::char_type*>(buffer.data()), fileSize );

    // Close the file
    file.close();

    return buffer;
  }

  Shader::Shader( vk::Device device, const std::string & vertexShaderFilename, const std::string & fragShaderFilename ) : device( device )
  {
    // Load the shader code from the file
    vertexModule   = createShaderModule( ReadFile( vertexShaderFilename) );
    fragmentModule = createShaderModule( ReadFile( fragShaderFilename ) );
  }

Shader::Shader( vk::Device device, const std::vector<uint8_t> & vertexShaderData, const std::vector<uint8_t> & fragShaderData ) : device( device )
  {
    // Load the shader code from the file
    vertexModule   = createShaderModule( vertexShaderData );
    fragmentModule = createShaderModule( fragShaderData );
  }

  Shader::~Shader()
  {
    // Destroy the shader modules
    device.destroyShaderModule( vertexModule );
    device.destroyShaderModule( fragmentModule );

    spdlog::info( "Destroying shader" );
  }

  auto Shader::createShaderModule( const std::vector<uint8_t> & code ) const -> vk::ShaderModule
  {
    // Create the shader module
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize( code.size() );
    createInfo.setPCode( reinterpret_cast<const uint32_t *>( code.data() ) );

    return device.createShaderModule( createInfo );
  }

  auto Shader::getPipelineShaderStageCreateInfos() const -> std::vector<vk::PipelineShaderStageCreateInfo>
  {
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

    vk::PipelineShaderStageCreateInfo vertexShaderStageInfo;
    vertexShaderStageInfo.setStage( vk::ShaderStageFlagBits::eVertex );
    vertexShaderStageInfo.setModule( vertexModule );
    vertexShaderStageInfo.pName = "main";
    shaderStages.push_back( vertexShaderStageInfo );

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
    fragShaderStageInfo.setStage( vk::ShaderStageFlagBits::eFragment );
    fragShaderStageInfo.setModule( fragmentModule );
    fragShaderStageInfo.pName = "main";
    shaderStages.push_back( fragShaderStageInfo );

    return shaderStages;
  }

}  // namespace braque
