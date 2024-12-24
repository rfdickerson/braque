//
// Created by Robert F. Dickerson on 12/24/24.
//

#ifndef FRAME_STATS_HPP
#define FRAME_STATS_HPP

#include <array>
#include <chrono>

namespace braque
{
  using Timestamp                   = std::chrono::time_point<std::chrono::high_resolution_clock>;
  constexpr int MAX_FRAME_LATENCIES = 100;
  using FrameLatencies              = std::array<float, MAX_FRAME_LATENCIES>;

  class FrameStats
  {
  public:
    FrameStats();

    void               update();

    // get all frame latencies
    [[nodiscard]] auto getLatencyData() const -> const FrameLatencies & { return frameLatencies; }
    [[nodiscard]] auto getLatencyDataOffset() const -> int { return currentFrameLatencyIndex; }

  private:
    // frame latencies
    FrameLatencies frameLatencies;
    int            currentFrameLatencyIndex = 0;
    Timestamp      lastFrameTime;
  };

}  // namespace braque

#endif  // FRAME_STATS_HPP
