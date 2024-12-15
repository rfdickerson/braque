//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef RENDERING_STAGE_HPP
#define RENDERING_STAGE_HPP

#include <vulkan/vulkan.hpp>

namespace braque {

class Renderer;
class Swapchain;

class RenderingStage {
public:
    RenderingStage(Renderer &renderer, Swapchain &swapchain);
    ~RenderingStage();

    void render();

private:
    Renderer& renderer;
    Swapchain& swapchain;

};

} // braque

#endif //RENDERING_STAGE_HPP
