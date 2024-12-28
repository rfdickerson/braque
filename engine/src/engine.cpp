//
// Created by Robert F. Dickerson on 12/13/24.
//

#include "braque/engine.h"

#include "braque/debug_window.h"
#include "braque/memory_allocator.h"
#include "braque/renderer.h"
#include "braque/rendering_stage.h"
#include "braque/swapchain.h"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace braque
{

  Engine::Engine()
    : swapchain( Swapchain( window, renderer ) )
    , memoryAllocator( MemoryAllocator( renderer ) )
    , uniforms_( Uniforms( *this ) )
    , renderingStage( RenderingStage( *this ) )
, debugWindow( DebugWindow( *this ) ),
scene_(Scene(memoryAllocator, renderer))
  {
    // Any other initialization after all members are constructed
    spdlog::info( "Engine created" );
    input_controller_.RegisterWindow(&window);
    input_controller_.RegisterObserver(&fps_controller_);
    fps_controller_.SetCamera(&camera_);
    input_controller_.RegisterObserver(&app_controller_);
    app_controller_.SetEngine(this);
  }

  Engine::~Engine()
  {
    renderer.waitIdle();
  }

  void Engine::run()
  {
    spdlog::info( "Starting the engine loop" );

    float accumulatedTime = 0.0f;
    constexpr float staticTimeStep = 1.0f / 165.0f;
    constexpr float max_latency = 0.25;

    while ( running )
    {
      
      swapchain.waitForFrame();
      swapchain.acquireNextImage();
      swapchain.waitForImageInFlight();
      // do drawing here

      // update the camera
      // update the input based on time left
      auto latency = swapchain.getFrameStats().Latency();
      latency = std::min<float>(latency, max_latency);

      accumulatedTime += latency;

      while (accumulatedTime >= staticTimeStep) {
        accumulatedTime -= staticTimeStep;
        input_controller_.PollEvents();
      }

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