//
// Created by Robert F. Dickerson on 12/19/24.
//
#include <fstream>
#include <spdlog/spdlog.h>

#include "braque/shader.hpp"

namespace braque {

// read file helper function
std::vector<char> ReadFile(const std::string &filename) {
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }

    // Get the size of the file and allocate a buffer
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    // Read the file into the buffer
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    // Close the file
    file.close();

    return buffer;
}


Shader::Shader(vk::Device device, const std::string &filename) : device(device) {
    // Load the shader code from the file
    vertexModule = createShaderModule(ReadFile(filename + ".vert.spv"));
    fragmentModule = createShaderModule(ReadFile(filename + ".frag.spv"));
}

Shader::~Shader() {
    // Destroy the shader modules
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);

    spdlog::info("Destroying shader");
}

auto Shader::createShaderModule(const std::vector<char> &code) const -> vk::ShaderModule {
    // Create the shader module
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(code.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t *>(code.data()));

    return device.createShaderModule(createInfo);
}

} // namespace braque
