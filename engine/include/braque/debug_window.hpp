//
// Created by Robert F. Dickerson on 12/15/24.
//

#ifndef DEBUG_WINDOW_HPP
#define DEBUG_WINDOW_HPP

#include <vulkan/vulkan.hpp>

namespace braque
{
  class Engine;
  class FrameStats;

  class DebugWindow
  {
  public:
    explicit DebugWindow( Engine & engine );
    ~DebugWindow();

    // remove copy and move
    DebugWindow( const DebugWindow & )                     = delete;
    auto operator=( const DebugWindow & ) -> DebugWindow & = delete;
    DebugWindow( DebugWindow && )                          = delete;
    auto operator=( DebugWindow && ) -> DebugWindow &      = delete;

    void        createFrame( FrameStats & frameStats ) const;
    static void renderFrame( const vk::CommandBuffer & commandBuffer );

  private:
    Engine & engine;

    static void initAssets();
  };

}  // namespace braque

#endif  // DEBUG_WINDOW_HPP
