#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"


#define COS_MAX_TILT_ANGLE 0.7071f
#define WORLD_UP_DIRECTION glm::vec3(0.f, 1.f, 0.f)


// Holds information about view and projection matrix
class Camera
{
public:
	static std::shared_ptr<Camera> createCamera(float width, float height,
		glm::vec3 &pos = glm::vec3(-5.f, 5.f, 5.f), glm::vec3 &at = glm::vec3(0.f),
		float fovy = 45.f, float near = 0.1f, float far = 100.f);

	virtual ~Camera() {}

	void moveLeft(float amount = .5f);
	void moveRight(float amount = .5f);
	void moveUp(float amount = .5f);
	void moveDown(float amount = .5f);
	void moveForward(float amount = .5f);
	void moveBackward(float amount = .5f);

	// In degrees
	void rotateLeft(float angle = 5.f);
	void rotateRight(float angle = 5.f);
	void rotateUp(float angle = 5.f);
	void rotateDown(float angle = 5.f);

	inline void updateViewMatrix() { view = glm::lookAt(position, position + aimDir, WORLD_UP_DIRECTION); }
	inline void updateProjMatrix() { proj = glm::perspective(fov, aspectRatio, nearClip, farClip); }

	glm::mat4 view;
	glm::mat4 proj;

	glm::vec3 position;
	glm::vec3 aimDir;
	float fov;
	float aspectRatio;
	float nearClip;
	float farClip;

protected:
	Camera(float w, float h, glm::vec3 &pos, glm::vec3 &at, float fovy, float near, float far);
};

#endif // CAMERA_H