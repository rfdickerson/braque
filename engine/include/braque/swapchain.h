//
// Created by Robert F. Dickerson on 12/13/24.
//

#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "frame_stats.h"
#include "renderer.h"
#include "braque/engine_context.h"
#include "window.h"
#include "braque/image.h"

namespace braque {

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class Swapchain {
 public:
  Swapchain(Window& window, EngineContext& context);
  ~Swapchain();

  // make sure copy and move are deleted
  Swapchain(const Swapchain&) = delete;
  auto operator=(const Swapchain&) -> Swapchain& = delete;
  Swapchain(Swapchain&&) = delete;
  auto operator=(Swapchain&&) -> Swapchain& = delete;

  [[nodiscard]] auto swapchain() const -> vk::SwapchainKHR {
    return swapchain_;
  }

  [[nodiscard]] auto swapchain_image() const -> vk::Image {
    return swapchainImages[currentImageIndex].GetImage();
  }

  [[nodiscard]] auto GetSwapchainImage() -> Image& {
    return swapchainImages[currentImageIndex];
  }

  [[nodiscard]] auto getCommandBuffer() const -> vk::CommandBuffer {
    return commandBuffers[currentImageIndex];
  }

  [[nodiscard]] auto getImageCount() const -> uint32_t { return imageCount; }

  [[nodiscard]] auto CurrentFrameIndex() const -> uint32_t {
    return currentFrameInFlight;
  }

  [[nodiscard]] static auto getFramesInFlightCount() -> uint32_t {
    return MAX_FRAMES_IN_FLIGHT;
  }

  [[nodiscard]] auto getImageView() const -> vk::ImageView {
    return swapchainImages[currentImageIndex].GetImageView();
  }

  [[nodiscard]] auto getExtent() const -> vk::Extent2D {
    return swapchainExtent;
  }

  [[nodiscard]] auto getFormat() const -> vk::Format { return swapchainFormat; }

  [[nodiscard]] auto getFrameStats() -> FrameStats& { return frameStats; }

  void waitForFrame() const;
  void acquireNextImage();
  void waitForImageInFlight();
  void submitCommandBuffer();
  void presentImage();

 private:
  vk::SwapchainKHR swapchain_;
  vk::SurfaceKHR surface_;

  EngineContext& context_;

  uint32_t imageCount = 2;
  uint32_t currentImageIndex = 0;
  uint32_t currentFrameInFlight = 0;
  std::vector<Image> swapchainImages;

  vk::Extent2D swapchainExtent;
  vk::Format swapchainFormat;

  // per swapchain image
  std::vector<vk::Semaphore> imageAvailableSemaphores;
  std::vector<vk::Semaphore> renderFinishedSemaphores;

  // per frame in flight
  std::vector<vk::Fence> inFlightFences;
  std::vector<vk::Fence> imagesInFlight;

  vk::CommandPool commandPool;
  std::vector<vk::CommandBuffer> commandBuffers;

  FrameStats frameStats;

  void createSwapchain(const Window& window);
  void createSemaphores();
  void createFences();
  void createSwapchainImages();
  void createCommandBuffers();
  // void createImageViews();
};

}  // namespace braque

#endif  // SWAPCHAIN_HPP
