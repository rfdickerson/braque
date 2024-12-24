//
// Created by Robert F. Dickerson on 12/24/24.
//

#include <braque/frame_stats.hpp>

namespace braque
{

  FrameStats::FrameStats(): frameLatencies{} {}

  void FrameStats::update()
  {
    // get current time and latency
    const auto currentTime = std::chrono::high_resolution_clock::now();
    // get the duration
    const auto frameTime = std::chrono::duration<float>( currentTime - lastFrameTime ).count();

    // update the frame latency
    frameLatencies[currentFrameLatencyIndex] = frameTime;
    currentFrameLatencyIndex                 = ( currentFrameLatencyIndex + 1 ) % MAX_FRAME_LATENCIES;
    lastFrameTime                            = currentTime;
  }

}  // namespace braque