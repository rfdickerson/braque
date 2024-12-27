#include <braque/camera.h>

namespace braque
{

constexpr glm::vec3 kWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

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
    newFront.x = glm::cos(yaw_) * glm::cos(pitch_);
    newFront.y = glm::sin(pitch_);
    newFront.z = glm::sin(yaw_) * glm::cos(pitch_);
    front_ = glm::normalize(newFront);
    // Always use the world up (0, 1, 0) to find right
    right_ = glm::normalize(glm::cross(front_, kWorldUp));
    up_ = glm::normalize(glm::cross(right_, front_));
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