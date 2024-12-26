//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef FPS_CONTROLLER_H
#define FPS_CONTROLLER_H

#include "braque/camera.hpp"
#include "braque/input/event_controller.h"

namespace braque {
class FirstPersonController : public EventController {
 public:
  FirstPersonController() = default;

  void SetCamera(Camera* camera) { camera_ = camera; }

  void OnMouseMoved(float x, float y) override;

 private:
  Camera* camera_ = nullptr;
  float mouse_sensitivity_ = 0.1f;
};
}  // namespace braque

#endif  //FPS_CONTROLLER_H
