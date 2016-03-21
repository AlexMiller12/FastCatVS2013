#include "AlexCamera.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

AlexCamera::AlexCamera()
{
}

AlexCamera::AlexCamera( float fieldOfView,
				uint screenWidth, 
				uint screenHeight,
				float nearClipPlane, 
				float farClipPlane )
{
	fov = fieldOfView;
	width = screenWidth;
	height = screenHeight;
	nearClip = nearClipPlane;
	farClip = farClipPlane;
	calcProjection();
}

AlexCamera::~AlexCamera()
{

}

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

void AlexCamera::lookAt( vec3 position, vec3 lookAt, vec3 worldUp )
{
	pos = position;
	center = lookAt;
	up = worldUp;
	view = glm::lookAt( pos, center, up );
}

mat4 AlexCamera::projectionMatrix()
{
	return projection;
}

mat4 AlexCamera::viewMatrix()
{
	return view;
}

mat4 AlexCamera::viewProjectionMatrix()
{	
	return projection * view;
}

//--------------------------------------------------------------------------HELPERS:

void AlexCamera::calcProjection()
{
	float aspect = 1.0f * width / height;
	projection = glm::perspective( fov, aspect, nearClip, farClip );
}