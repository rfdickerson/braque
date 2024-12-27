//
// Created by Robert F. Dickerson on 12/24/24.
//

#include <braque/frame_stats.h>

namespace braque {

FrameStats::FrameStats() : frame_latencies_{} {}

void FrameStats::Update() {
  // get current time and latency
  const auto currentTime = std::chrono::high_resolution_clock::now();
  // get the duration
  const auto frameTime =
      std::chrono::duration<TimeType>(currentTime - last_frame_time_).count();

  // update the frame latency
  frame_latencies_[current_frame_latency_index_] = frameTime;
  current_frame_latency_index_ =
      (current_frame_latency_index_ + 1) % kMaxFrameLatencies;
  last_frame_time_ = currentTime;
}

}  // namespace braque