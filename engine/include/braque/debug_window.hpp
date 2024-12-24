//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef DEBUG_WINDOW_HPP
#define DEBUG_WINDOW_HPP

#include "frame_stats.hpp"

#include <vulkan/vulkan.hpp>

namespace braque {
class Engine;

class DebugWindow {
public:
    DebugWindow(Engine& engine);
    ~DebugWindow();

    // remove copy and move
    DebugWindow(const DebugWindow&) = delete;
    DebugWindow& operator=(const DebugWindow&) = delete;
    DebugWindow(DebugWindow&&) = delete;
    DebugWindow& operator=(DebugWindow&&) = delete;

    void createFrame(FrameStats& frameStats) const;
    static void renderFrame(const vk::CommandBuffer& commandBuffer);

private:
    Engine& engine;

    static void initAssets();
};

} // braque

#endif //DEBUG_WINDOW_HPP
