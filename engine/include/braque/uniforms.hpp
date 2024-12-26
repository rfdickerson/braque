#pragma once

#include "braque/memory_allocator.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace braque
{

  class Engine;

  class Uniforms
  {
  public:
    Uniforms( Engine & engine );
    ~Uniforms();

    // remove copy and move
        Uniforms( const Uniforms & )                     = delete;

    // void SetCameraData(Camera& camera);

  private:
    Engine & engine_;

    std::vector<AllocatedBuffer> cameraBuffers;
    std::vector<vk::DescriptorSet> descriptorSets;

    vk::DescriptorSetLayout      descriptorSetLayout;
    vk::DescriptorPool          descriptorPool;

    void createUniformBuffers();
    void createDescriptorSetLayout();
    void createDescriptorPool();
    void createDescriptorSets();
  };
}  // namespace braque