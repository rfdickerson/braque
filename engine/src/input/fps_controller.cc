//
// Created by Robert F. Dickerson on 12/26/24.
//

#include "braque/input/fps_controller.h"

#include <GLFW/glfw3.h>

namespace braque
{
  void FirstPersonController::OnMouseMoved(float xoffset, float yoffset) {

    if (camera_ == nullptr) {
      return;
    }

    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    camera_->yaw_ += xoffset;
    camera_->pitch_ += yoffset;


    if ( camera_->pitch_ > 89.0f )
    camera_->pitch_ = 89.0f;
    if ( camera_->pitch_ < -89.0f )
    camera_->pitch_ = -89.0f;

    camera_->UpdateCameraVectors();
  }

void FirstPersonController::OnKeyPressed(int key) {
  if (camera_ == nullptr) {
    return;
  }

  const auto velocity = 2.5f * 0.1f;

  if (key == GLFW_KEY_W) {
    camera_->position_ += camera_->front_ * velocity;
  }
  if (key == GLFW_KEY_S) {
    camera_->position_ -= camera_->front_ * velocity;
  }
  if (key == GLFW_KEY_A) {
    camera_->position_ -= camera_->right_ * velocity;
  }
  if (key == GLFW_KEY_D) {
    camera_->position_ += camera_->right_ * velocity;
  }
}

} // namespace braque