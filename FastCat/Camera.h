#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"


#define COS_MAX_TILT_ANGLE 0.7071f
#define WORLD_UP_DIRECTION glm::vec3(0.f, 1.f, 0.f)

using namespace glm;
// Holds information about view and projection matrix
class Camera
{
	
//---------------------------------------------------------------------------FIELDS:

public:
	glm::mat4 view;
	glm::mat4 proj;
	
private:
	glm::vec3 position;
	glm::vec3 aimDir;
	float fov;
	float aspectRatio;
	float nearClip;
	float farClip;
		
//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
		
public:

	virtual ~Camera() {}
	
protected:
	Camera( float w, float h, 
			vec3 &pos, vec3 &at, float fovy, 
			float near, float far );
	
//--------------------------------------------------------------------------METHODS:

public:
	static std::shared_ptr<Camera> createCamera( float screenWidth, 
												 float screenHeight,
												 vec3 &pos = vec3(-5.f, 5.f, 5.f), 
												 vec3 &at = vec3(0.f),
												 float fovy = 45.f, 
												 float nearClp = 0.1f, 
												 float farClp = 100.f );
	void moveBackward( float amount = .5f );
	void moveDown( float amount = .5f );
	void moveForward( float amount = .5f );
	void moveLeft(float amount = .5f);
	void moveRight(float amount = .5f);
	void moveUp(float amount = .5f);

	// In degrees
	void rotateDown( float angle = 5.f );
	void rotateLeft(float angle = 5.f);
	void rotateRight(float angle = 5.f);
	void rotateUp(float angle = 5.f);

	inline void updateViewMatrix() 
	{ 
		view = glm::lookAt( position, position + aimDir, WORLD_UP_DIRECTION );
	}
	
	inline void updateProjMatrix() { 
		proj = glm::perspective( fov, aspectRatio, nearClip, farClip );
	}

};

#endif // CAMERA_H