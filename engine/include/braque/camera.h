#ifndef BRAQUE_CAMERA_H_
#define BRAQUE_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace braque
{
enum CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
  public:
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f),
	       glm::vec3 up  = glm::vec3(0.0f, 1.0f, 0.0f),
	       float yaw = -90.0f, float pitch = 0.0f);

	glm::mat4 ViewMatrix() const;
	glm::mat4 ProjectionMatrix() const;

	// void ProcessKeyboard(CameraMovement direction, float deltaTime);
	//
	// void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	//
	// void ProcessMouseScroll(float yoffset);

	void UpdateCameraVectors();

	void LookAt(glm::vec3 target);

  public:

	glm::vec3 position_;
	glm::vec3 front_;
	glm::vec3 up_;
    glm::vec3 right_;
	float     yaw_;
	float     pitch_;
	float     fov_;
	float     aspectRatio_;
	float     nearPlane_;
	float     farPlane_;
};
}; // namespace obsidian

#endif // BRAQUE_CAMERA_H_
