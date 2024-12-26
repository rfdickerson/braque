#include <braque/camera.hpp>

namespace braque
{

  Camera::Camera( glm::vec3 pos, glm::vec3 up, float yaw, float pitch )
      : position_( pos ),
        front_( glm::vec3( 0.0f, 0.0f, -1.0f ) ),
        up_( up ),
        yaw_( yaw ),
        pitch_( pitch ),
        fov_( 45.0f ),
        aspectRatio_( 800.0f / 600.0f ),
        nearPlane_( 0.1f ),
        farPlane_( 100.0f )
  {
    UpdateCameraVectors();
  }

  void Camera::UpdateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    newFront.y = sin(glm::radians(pitch_));
    newFront.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(newFront);
    right_ = glm::normalize(glm::cross(front_, up_));
    up_ = glm::normalize(glm::cross(right_, front_));
    }

    glm::mat4 Camera::ViewMatrix() const
    {
      return glm::lookAt( position_, position_ + front_, up_ );
    }

    glm::mat4 Camera::ProjectionMatrix() const
    {
      return glm::perspective( glm::radians( fov_ ), aspectRatio_, nearPlane_, farPlane_ );
    }

    void Camera::ProcessKeyboard( CameraMovement direction, float deltaTime )
    {
      const auto velocity = 2.5f * deltaTime;

      if ( direction == FORWARD )
      position_ += front_ * velocity;
      if ( direction == BACKWARD )
      position_ -= front_ * velocity;
      if ( direction == LEFT )
      position_ -= right_ * velocity;
      if ( direction == RIGHT )
      position_ += right_ * velocity;
    }

    void Camera::ProcessMouseMovement( float xoffset, float yoffset, bool constrainPitch )
    {
      xoffset *= 0.1f;
      yoffset *= 0.1f;

      yaw_ += xoffset;
      pitch_ += yoffset;

      if ( constrainPitch )
      {
        if ( pitch_ > 89.0f )
        pitch_ = 89.0f;
        if ( pitch_ < -89.0f )
        pitch_ = -89.0f;
      }

      UpdateCameraVectors();
    }


} // namespace braque