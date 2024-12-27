//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef FPS_CONTROLLER_H
#define FPS_CONTROLLER_H

#include "braque/camera.h"
#include "braque/input/event_controller.h"

namespace braque {
class FirstPersonController final : public EventController {
 public:
  FirstPersonController() = default;

  void SetCamera(Camera* camera) { camera_ = camera; }

  void OnMouseMoved(float x, float y) override;
  void OnKeyPressed(int key) override;

 private:
  Camera* camera_ = nullptr;
  float mouse_sensitivity_ = 0.001f;
  float movement_speed_ = 0.25f;
};
}  // namespace braque

#endif  //FPS_CONTROLLER_H
