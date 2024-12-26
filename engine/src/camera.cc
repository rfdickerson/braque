#include <braque/camera.h>

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
    newFront.x = cos(yaw_) * cos(pitch_);
    newFront.y = sin(pitch_);
    newFront.z = sin(yaw_) * cos(pitch_);
    front_ = normalize(newFront);
    right_ = normalize(cross(front_, up_));
    up_ = normalize(cross(right_, front_));
    }

    auto Camera::ViewMatrix() const -> glm::mat4
    {
      return lookAt( position_, position_ + front_, up_ );
    }

    auto Camera::ProjectionMatrix() const -> glm::mat4
    {
      return glm::perspective( glm::radians( fov_ ), aspectRatio_, nearPlane_, farPlane_ );
    }

} // namespace braque