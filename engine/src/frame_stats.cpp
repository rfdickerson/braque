//
// Created by Robert F. Dickerson on 12/24/24.
//

#include <braque/frame_stats.h>

#include <numeric>

namespace braque {

FrameStats::FrameStats()
    : current_frame_latency_index_(0)
    , current_tick_(0)
    , accumulated_ns_(0)
    , last_frame_time_(std::chrono::high_resolution_clock::now())
{
  // Initialize all frame latencies to 0
  std::fill(frame_latencies_.begin(), frame_latencies_.end(), 0.0f);
}

float FrameStats::Latency() const {
  // Return the average latency of all recorded frame latencies
  if (frame_latencies_[0] == 0.0f) {
    return 0.0f; // No data recorded yet
  }
  return std::accumulate(frame_latencies_.begin(), frame_latencies_.end(), 0.0f) / MAX_FRAME_LATENCIES;
}

const FrameStats::FrameLatencies& FrameStats::LatencyData() const {
  // Simply return a const reference to the frame_latencies_ array
  return frame_latencies_;
}

uint32_t FrameStats::LatencyDataOffset() const {
  // Return the current index in the circular buffer
  // This is useful for drawing the data in the correct order if it's being used for a rolling graph
  return current_frame_latency_index_;
}

void FrameStats::Update() {
  auto current_time = std::chrono::high_resolution_clock::now();
  float frame_time = std::chrono::duration<float, std::milli>(current_time - last_frame_time_).count();

  // if (frame_time < 0.1f) {
  //   return;
  // }

  frame_latencies_[current_frame_latency_index_] = frame_time;
  current_frame_latency_index_ = (current_frame_latency_index_ + 1) % MAX_FRAME_LATENCIES;

  uint64_t elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - last_frame_time_).count();
  accumulated_ns_ = std::min(accumulated_ns_ + elapsed_ns, MAX_ACCUMULATED_NS);

  last_frame_time_ = current_time;

}

uint32_t FrameStats::GetTicksToProcess() const {
  return static_cast<uint32_t>(accumulated_ns_ / NANOSECONDS_PER_TICK);
}

void FrameStats::ConsumeTime(uint32_t ticks) {
  accumulated_ns_ -= ticks * NANOSECONDS_PER_TICK;
  current_tick_ += ticks;
}

float FrameStats::GetFPS() const {
  if (frame_latencies_[0] == 0.0f) return 0.0f;
  float avgLatency = std::accumulate(frame_latencies_.begin(), frame_latencies_.end(), 0.0f) / MAX_FRAME_LATENCIES;
  return 1000.0f / avgLatency;
}

uint64_t FrameStats::GetDeltaTime() const {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::high_resolution_clock::now() - last_frame_time_).count();
}


float FrameStats::GetAverageFrameTime() const {
  return std::accumulate(frame_latencies_.begin(), frame_latencies_.end(), 0.0f) / MAX_FRAME_LATENCIES;
}

void FrameStats::Reset() {
  std::fill(frame_latencies_.begin(), frame_latencies_.end(), 0.0f);
  current_frame_latency_index_ = 0;
  current_tick_ = 0;
  accumulated_ns_ = 0;
  last_frame_time_ = std::chrono::high_resolution_clock::now();
}

uint64_t FrameStats::GetFrameCount() const {
  return current_tick_ / (TICKS_PER_SECOND / 60);  // Assuming 60 fps
}


}  // namespace braque