#pragma once

#include "braque/memory_allocator.hpp"

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
  };
}  // namespace braque