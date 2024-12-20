//
// Created by Robert F. Dickerson on 12/19/24.
//

#ifndef SHADER_HPP
#define SHADER_HPP

#include <vulkan/vulkan.hpp>

namespace braque {

class Shader {
public:
    Shader(vk::Device device, const std::string &filename);
    ~Shader();

private:
    vk::Device device;
    vk::ShaderModule vertexModule;
    vk::ShaderModule fragmentModule;

    vk::ShaderModule createShaderModule(const std::vector<char> &code) const;
};

} // namespace braque

#endif //SHADER_HPP
