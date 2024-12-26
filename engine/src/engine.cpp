//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.h"

#include "braque/debug_window.h"
#include "braque/memory_allocator.h"
#include "braque/renderer.h"
#include "braque/rendering_stage.h"
#include "braque/swapchain.h"
#include "braque/window.h"

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
    spdlog::info( "Engine created" );
    input_contoller_.RegisterWindow(&window);
    input_contoller_.RegisterObserver(&fps_controller_);
    fps_controller_.SetCamera(&camera_);
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
      
      swapchain.waitForFrame();
      swapchain.acquireNextImage();
      swapchain.waitForImageInFlight();
      // do drawing here

      // update the camera
      input_contoller_.PollEvents();

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