#ifndef FC_RENDERER
#define FC_RENDERER

//Include GLEW  
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include <memory>
#include <stdio.h>  
#include <stdlib.h>  
#include <Windows.h>

#include "CCLevel.h"
#include "Camera.h"

class FastCatRenderer
{
	
//----------------------------------------------------------------------------ENUMS:

public:
	enum FastCatStates
	{
		NEUTRAL, TESTING
	};

//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:

public:
	std::shared_ptr<CCLevel> testMesh;
	bool isReady;
	std::shared_ptr<Camera> camera;

private:
	GLFWwindow* window;
	FastCatStates state;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	FastCatRenderer();
	virtual ~FastCatRenderer();
	
//--------------------------------------------------------------------------METHODS:

public:
	void closeWindow();
	void createWindow();
	void init();

	void render(); //TODO will be private if loop lives in this class
	bool shouldWindowClose();
	void test();

private:
	void testPass();
};

#endif