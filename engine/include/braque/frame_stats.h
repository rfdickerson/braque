//
// Created by Robert F. Dickerson on 12/24/24.
//

#ifndef FRAME_STATS_HPP
#define FRAME_STATS_HPP

#include <array>
#include <chrono>

namespace braque {

using TimeType = uint64_t;

static constexpr uint32_t TICKS_PER_SECOND = 25200;
static constexpr uint64_t NANOSECONDS_PER_TICK = 1'000'000'000ULL / TICKS_PER_SECOND;
static constexpr TimeType SECONDS_PER_TICK = 1.0 / TICKS_PER_SECOND;

class FrameStats {
 public:

  static constexpr size_t MAX_FRAME_LATENCIES = 100;
  using FrameLatencies = std::array<float, MAX_FRAME_LATENCIES>;

  FrameStats();

  void Update();

  [[nodiscard]] auto Latency() const -> float;
  [[nodiscard]] auto LatencyData() const -> const FrameLatencies&;
  [[nodiscard]] auto LatencyDataOffset() const -> uint32_t;

  // Add tick-specific methods
  [[nodiscard]] uint64_t GetCurrentTick() const { return current_tick_; }
  [[nodiscard]] uint32_t GetTicksToProcess() const;
  void ConsumeTime(uint32_t ticks);

  // helper methods for common refresh rates
  static constexpr uint32_t GetTicksPerFrame(uint32_t refresh_rate) {
    return TICKS_PER_SECOND / refresh_rate;
  }

  static constexpr uint32_t TICKS_60HZ() { return GetTicksPerFrame(60);};
  static constexpr uint32_t TICKS_144HZ() { return GetTicksPerFrame(144); }
  static constexpr uint32_t TICKS_120HZ() { return GetTicksPerFrame(120); }
  static constexpr uint32_t TICKS_240HZ() { return GetTicksPerFrame(240); }

  [[nodiscard]] float GetFPS() const;
  [[nodiscard]] uint64_t GetDeltaTime() const;
  [[nodiscard]] float GetAverageFrameTime() const;
  void Reset();
  [[nodiscard]] uint64_t GetFrameCount() const;

  private:
  FrameLatencies frame_latencies_;
  int current_frame_latency_index_ = 0;
  std::chrono::high_resolution_clock::time_point last_frame_time_;

  // new tick related members
  uint64_t current_tick_ = 0;
  uint64_t accumulated_ns_ = 0;
  static constexpr uint64_t MAX_ACCUMULATED_NS = NANOSECONDS_PER_TICK * 100; // prevent spiral of death

};

}  // namespace braque

#endif  // FRAME_STATS_HPP
