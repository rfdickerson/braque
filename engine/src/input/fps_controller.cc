//
// Created by Robert F. Dickerson on 12/26/24.
//

#include "braque/input/fps_controller.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>

namespace braque
{

  constexpr float PI_2 = glm::pi<float>() / 2.0f;

  void FirstPersonController::OnMouseMoved(float xoffset, float yoffset) {

    if (camera_ == nullptr) {
      return;
    }

    // validate xoffset and yoffset
    if (xoffset == 0 && yoffset == 0) {
      return;
    }

    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    camera_->yaw_ += xoffset;
    camera_->pitch_ += yoffset;

    camera_->pitch_ = std::min(camera_->pitch_, PI_2);
    camera_->pitch_ = std::max(camera_->pitch_, -PI_2);

    camera_->UpdateCameraVectors();
  }

void FirstPersonController::OnKeyPressed(int key) {
  if (camera_ == nullptr) {
    return;
  }

  if (key == GLFW_KEY_W) {
    camera_->position_ += camera_->front_ * movement_speed_;
  }
  if (key == GLFW_KEY_S) {
    camera_->position_ -= camera_->front_ * movement_speed_;
  }
  if (key == GLFW_KEY_A) {
    camera_->position_ -= camera_->right_ * movement_speed_;
  }
  if (key == GLFW_KEY_D) {
    camera_->position_ += camera_->right_ * movement_speed_;
  }
}

} // namespace braque