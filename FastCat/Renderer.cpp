#include "Renderer.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

Renderer::Renderer()
{}

Renderer::~Renderer()
{}

//------------------------------------------------------------------------FUNCTIONS:

void defaultErrorCallback( int error, const char* description )
{
	fputs( description, stderr );
	_fgetchar();
}

// Default key input callback  
void defaultKeyCallback( GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{
		glfwSetWindowShouldClose( window, GL_TRUE );
	}
}

//--------------------------------------------------------------------------METHODS:

// Binds to the window's gl context
void Renderer::bind()
{
	// TODO if(window) else, error
	glfwMakeContextCurrent( window );
}

void Renderer::closeWindow()
{
	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow( window );

	//Finalize and clean up GLFW  
	glfwTerminate();
}

// Creates a GLFW window and initializes GL context. Returns true upon success
bool Renderer::createWindow()
{
	//Set the error callback  
	glfwSetErrorCallback( defaultErrorCallback );
	//Initialize GLFW  
	if( !glfwInit() )
	{
		return false;
	}
	//Create a window and create its OpenGL context  
	window = glfwCreateWindow( DEFAULT_WINDOW_WIDTH,
		DEFUALT_WINDOW_HEIGHT,
		DEFAULT_WINDOW_NAME,
		NULL,
		NULL );
	//If the window couldn't be created  
	if( !window )
	{
		glfwTerminate();
		return false;
	}
	// Make the context of the specified window current on the calling thread   
	glfwMakeContextCurrent( window );
	//Sets the key callback  
	glfwSetKeyCallback( window, defaultKeyCallback );
	//Initialize GLEW  
	GLenum glewInitStatus = glewInit();
	// Return true if status is OK
	return glewInitStatus == GLEW_OK;
}

bool Renderer::shouldClose()
{
	return glfwWindowShouldClose( window );
}

void Renderer::swapBuffers()
{
	// update other events like input handling which tend to be done now
	glfwPollEvents();
	glfwSwapBuffers( window );
}

void Renderer::unbind()
{
	glfwMakeContextCurrent( NULL );
}

//--------------------------------------------------------------------------HELPERS:

