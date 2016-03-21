#ifndef MY_GL_RENDERER
#define MY_GL_RENDERER

#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include <stdio.h>  
#include <stdlib.h>  

#include <Windows.h>

class Renderer
{
	
//----------------------------------------------------------------------------ENUMS:

//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:
	
protected:
	GLFWwindow* window;

private:	
	const int DEFAULT_WINDOW_WIDTH	= 640;
	const int DEFUALT_WINDOW_HEIGHT = 480;
	const char* DEFAULT_WINDOW_NAME = "OpenGL Window";

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	Renderer();
	virtual ~Renderer();
	
//--------------------------------------------------------------------------METHODS:

public:
	void bind();
	void closeWindow();
	bool createWindow();
	bool shouldClose();
	void swapBuffers();
	void unbind();

private:

//--------------------------------------------------------------GETTERS AND SETTERS:

public:
	
};

#endif