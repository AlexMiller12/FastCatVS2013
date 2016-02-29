#ifndef FC_RENDERER
#define FC_RENDERER

//Include GLEW  
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include <stdio.h>  
#include <stdlib.h>  

#include <Windows.h>

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
	void render(); //TODO will be private if loop lives in this class
	bool shouldWindowClose();
	void test();

private:
	void testPass();
};

#endif