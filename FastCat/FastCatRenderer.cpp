
#include "FastCatRenderer.h"
#include "MyUtil.h"

#include <stdio.h>  
#include <stdlib.h>  

#include <Windows.h>
//Include GLEW  
#include "Dependencies\glew\glew.h"

//Include GLFW  
#include "Dependencies\glfw\glfw3.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:


FastCatRenderer::FastCatRenderer()
{}

FastCatRenderer::~FastCatRenderer()
{}

//--------------------------------------------------------------------------METHODS:

//Define the key input callback  
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GL_TRUE );
}
//Define an error callback  
static void error_callback( int error, const char* description )
{
	fputs( description, stderr );
	_fgetchar();
}

void FastCatRenderer::init()
{
	//Set the error callback  
	glfwSetErrorCallback( error_callback );

	//Initialize GLFW  
	if( ! glfwInit() )
	{
	}

	//Declare a window object  
	GLFWwindow* window;

	//Create a window and create its OpenGL context  
	window = glfwCreateWindow( 640, 480, "Test Window", NULL, NULL );

	//If the window couldn't be created  
	if( ! window )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent( window );

	//Sets the key callback  
	glfwSetKeyCallback( window, key_callback );

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if( err != GLEW_OK )
	{
		fprintf( stderr, "Error: %s\n", glewGetErrorString( err ) );
		return;
	}

	//Set a background color  
	glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );

	//Main Loop  
	do
	{
		//Clear color buffer  
		glClear( GL_COLOR_BUFFER_BIT );

		//Swap buffers  
		glfwSwapBuffers( window );
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

	} //Check if the ESC key had been pressed or if the window had been closed  
	while( !glfwWindowShouldClose( window ) );

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow( window );
	//Finalize and clean up GLFW  
	glfwTerminate();
}

//TODO: temp
void FastCatRenderer::test()
{
	print();
}

//--------------------------------------------------------------------------HELPERS:

//TODO TEMP:
int rot = 20; 
// Function to render hello world (to be passed to glut)
void FastCatRenderer::testPass()
{

}
