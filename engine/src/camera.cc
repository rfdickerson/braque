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
        fov_( 65.0f ),
        aspectRatio_( 800.0f / 600.0f ),
        nearPlane_( 0.1f ),
        farPlane_( 10000.0f )
  {
    UpdateCameraVectors();
  }

  void Camera::UpdateCameraVectors() {

    const auto v_x = glm::cos(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));
    const auto v_y = glm::sin(glm::radians(pitch_));
    const auto v_z = glm::sin(glm::radians(yaw_)) * glm::cos(glm::radians(pitch_));

    const auto new_front = glm::vec3(v_x, v_y, v_z);

    front_ = normalize(new_front);
    // Always use the world up (0, 1, 0) to find right
    right_ = normalize(cross(front_, kWorldUp));
    up_ = normalize(cross(right_, front_));
    }

    auto Camera::ViewMatrix() const -> glm::mat4
    {
      return lookAt( position_, position_ - front_, up_ );
    }

    auto Camera::ProjectionMatrix() const -> glm::mat4
    {
      // For reversed depth, we use near=1.0 and far=0.0
      auto projection = glm::perspective(glm::radians(fov_), aspectRatio_, nearPlane_, farPlane_);
      //
      // // Reverse the z-direction (modify the depth mapping)
      projection[2][2] *= -1.0f; // Flip the sign of the z-term
      projection[2][3] *= -1.0f; // Flip the sign of the translation in z

      projection[1][1] *= -1; // Flip Y-axis for Vulkan

      return projection;
    }

} // namespace braque