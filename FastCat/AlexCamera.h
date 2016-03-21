#ifndef MY_CAMERA
#define MY_CAMERA

#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"

using namespace glm;

class AlexCamera
{
	
//----------------------------------------------------------------------------ENUMS:
	
//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:
	
private:
	float fov;
	float nearClip;
	float farClip;

	uint width, height;

	vec3 pos;
	vec3 center;
	vec3 up;

	mat4 view;
	mat4 projection;


//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
		
public:
	AlexCamera();
	AlexCamera( float fieldOfView,
			uint screenWidth, 
			uint screenHeight, 
			float nearClipPlane, 
			float farClipPlane );
	~AlexCamera();

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

public:
	void lookAt( vec3 camPos, vec3 lookAt, vec3 worldUp = vec3( 0, 1, 0 ) );
	mat4 projectionMatrix();
	mat4 viewMatrix();
	mat4 viewProjectionMatrix();

private:
	void calcProjection();
};

#endif