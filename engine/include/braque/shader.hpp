//
// Created by Robert F. Dickerson on 12/19/24.
//

#ifndef SHADER_HPP
#define SHADER_HPP

#include <vulkan/vulkan.hpp>

namespace braque {

class Shader {
public:
    Shader(vk::Device device, const std::string &vertShaderFilename, const std::string &fragShaderFilename);
    ~Shader();

    // remove copy and move
    Shader(const Shader &) = delete;
        auto operator=(const Shader &) -> Shader & = delete;
        Shader(Shader &&) = delete;


    [[nodiscard]] auto getPipelineShaderStageCreateInfos() const -> std::vector<vk::PipelineShaderStageCreateInfo>;

private:
    vk::Device device;
    vk::ShaderModule vertexModule;
    vk::ShaderModule fragmentModule;

    auto createShaderModule(const std::vector<char> &code) const -> vk::ShaderModule;
};

} // namespace braque

#endif //SHADER_HPP
