//
// Created by Robert F. Dickerson on 12/26/24.
//

#include "braque/input/fps_controller.h"

namespace braque
{
  void FirstPersonController::OnMouseMoved(float xoffset, float yoffset) {

    if (camera_ == nullptr) {
      return;
    }

    xoffset *= 0.1f;
    yoffset *= 0.1f;

    camera_->yaw_ += xoffset;
    camera_->pitch_ += yoffset;


    if ( camera_->pitch_ > 89.0f )
    camera_->pitch_ = 89.0f;
    if ( camera_->pitch_ < -89.0f )
    camera_->pitch_ = -89.0f;

    camera_->UpdateCameraVectors();
  }

} // namespace braque