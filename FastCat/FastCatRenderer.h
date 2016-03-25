#ifndef FC_RENDERER
#define FC_RENDERER

//Include GLEW  
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include <memory>
#include <stdio.h>  
#include <stdlib.h>  
#include <Windows.h>

#include "Renderer.h"
#include "ControlMesh.h"
#include "Camera.h"

class FastCatRenderer : Renderer
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
	std::shared_ptr<ControlMesh> testMesh;
	bool isReady;
	std::shared_ptr<Camera> camera;

private:
	FastCatStates state;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	FastCatRenderer();
	virtual ~FastCatRenderer();
	
//--------------------------------------------------------------------------METHODS:

public:
	void init();
	void render();
	void test();

private:
	void testPass();
};

#endif