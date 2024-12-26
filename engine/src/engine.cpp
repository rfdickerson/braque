//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.hpp"

#include "braque/debug_window.hpp"
#include "braque/memory_allocator.hpp"
#include "braque/renderer.hpp"
#include "braque/rendering_stage.hpp"
#include "braque/swapchain.hpp"
#include "braque/window.hpp"

#include <spdlog/spdlog.h>

namespace braque
{

  Engine::Engine()
    : swapchain( Swapchain( window, renderer ) )
    , memoryAllocator( MemoryAllocator( renderer ) )
    , uniforms_( Uniforms( *this ) )
    , renderingStage( RenderingStage( *this ) )
, debugWindow( DebugWindow( *this ) )
  {
    // Any other initialization after all members are constructed
  }

  Engine::~Engine()
  {
    renderer.waitIdle();
  }

  void Engine::run()
  {
    spdlog::info( "Starting the engine loop" );

    while ( !window.ShouldClose() )
    {
      Window::PollEvents();

      swapchain.waitForFrame();
      swapchain.acquireNextImage();
      swapchain.waitForImageInFlight();
      // do drawing here

      // update the camera
      camera_.UpdateCameraVectors();

      debugWindow.createFrame( swapchain.getFrameStats() );

      auto commandBuffer = swapchain.getCommandBuffer();
      RenderingStage::begin( commandBuffer );
      uniforms_.SetCameraData(commandBuffer, camera_);
      renderingStage.prepareImageForColorAttachment( commandBuffer );
      renderingStage.beginRenderingPass( commandBuffer );
      uniforms_.Bind( commandBuffer );
      renderingStage.renderTriangle( commandBuffer );
      DebugWindow::renderFrame( commandBuffer );
      RenderingStage::endRenderingPass( commandBuffer );
      renderingStage.prepareImageForDisplay( commandBuffer );
      RenderingStage::end( commandBuffer );

      swapchain.submitCommandBuffer();
      swapchain.presentImage();
    }
  }

}  // namespace braque