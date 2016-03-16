#include "Camera.h"
#include "Dependencies/glm/gtx/rotate_vector.hpp"


std::shared_ptr<Camera> Camera::createCamera(float width, float height,
	glm::vec3 & pos, glm::vec3 & at, float fovy, float near, float far)
{
	return std::shared_ptr<Camera>(new Camera(width, height, pos, at, fovy, near, far));
}


Camera::Camera(float w, float h, glm::vec3 & pos, glm::vec3 & at,
	float fovy, float near, float far)
	: position(pos), aimDir(glm::normalize(at - pos)), fov(fovy), aspectRatio(w / h),
	nearClip(near), farClip(far)
{
	updateViewMatrix();
	updateProjMatrix();
}


void Camera::moveLeft(float amount)
{
	glm::vec3 right = glm::normalize(glm::cross(WORLD_UP_DIRECTION, -aimDir));
	position -= amount * right;
	updateViewMatrix();
}


void Camera::moveRight(float amount)
{
	glm::vec3 right = glm::normalize(glm::cross(WORLD_UP_DIRECTION, -aimDir));
	position += amount * right;
	updateViewMatrix();
}


void Camera::moveUp(float amount)
{
	position += amount * WORLD_UP_DIRECTION;
	updateViewMatrix();
}


void Camera::moveDown(float amount)
{
	position -= amount * WORLD_UP_DIRECTION;
	updateViewMatrix();
}


void Camera::moveForward(float amount)
{
	position += amount * aimDir;
	updateViewMatrix();
}


void Camera::moveBackward(float amount)
{
	position -= amount * aimDir;
	updateViewMatrix();
}


void Camera::rotateLeft(float angle)
{
	aimDir = glm::rotate(aimDir, angle, WORLD_UP_DIRECTION);
	updateViewMatrix();
}


void Camera::rotateRight(float angle)
{
	aimDir = glm::rotate(aimDir, -angle, WORLD_UP_DIRECTION);
	updateViewMatrix();
}


void Camera::rotateUp(float angle)
{
	if (glm::dot(aimDir, WORLD_UP_DIRECTION) < COS_MAX_TILT_ANGLE)
	{
		glm::vec3 right = glm::normalize(glm::cross(WORLD_UP_DIRECTION, -aimDir));
		aimDir = glm::rotate(aimDir, angle, right);
		updateViewMatrix();
	}
}


void Camera::rotateDown(float angle)
{
	if (glm::dot(aimDir, WORLD_UP_DIRECTION) > -COS_MAX_TILT_ANGLE)
	{
		glm::vec3 right = glm::normalize(glm::cross(WORLD_UP_DIRECTION, -aimDir));
		aimDir = glm::rotate(aimDir, -angle, right);
		updateViewMatrix();
	}
}