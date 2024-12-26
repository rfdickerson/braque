//
// Created by Robert F. Dickerson on 12/24/24.
//

#ifndef FRAME_STATS_HPP
#define FRAME_STATS_HPP

#include <array>
#include <chrono>

namespace braque {
using Timestamp = std::chrono::time_point<std::chrono::high_resolution_clock>;
constexpr int MAX_FRAME_LATENCIES = 100;
using FrameLatencies = std::array<float, MAX_FRAME_LATENCIES>;

class FrameStats {
 public:
  FrameStats();

  void Update();

  // get all frame latencies
  [[nodiscard]] auto LatencyData() const -> const FrameLatencies& {
    return frame_latencies_;
  }
  [[nodiscard]] auto LatencyDataOffset() const -> int {
    return current_frame_latency_index_;
  }

 private:
  // frame latencies
  FrameLatencies frame_latencies_;
  int current_frame_latency_index_ = 0;
  Timestamp last_frame_time_;
};

}  // namespace braque

#endif  // FRAME_STATS_HPP
