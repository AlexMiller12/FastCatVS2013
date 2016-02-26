
#include "FastCatRenderer.h"

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
		exit( EXIT_FAILURE );
	}

	//Set the GLFW window creation hints - these are optional  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  

	//Declare a window object  
	GLFWwindow* window;

	//Create a window and create its OpenGL context  
	window = glfwCreateWindow( 640, 480, "Test Window", NULL, NULL );

	//If the window couldn't be created  
	if( ! window )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
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

	exit( EXIT_SUCCESS );
}

//TODO: temp

void FastCatRenderer::test()
{ 
	
	
}

//--------------------------------------------------------------------------HELPERS:

//TODO TEMP:
int rot = 20; 
// Function to render hello world (to be passed to glut)
void FastCatRenderer::testPass()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -2.0, 2.0, -2.0, 2.0, -2.0, 500.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 2, 2, 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glScalef( .005, .005, .005 );
	glRotatef( rot++, 0, 1, 0 );
	glRotatef( 30, 0, 0, 1 );
	glRotatef( 5, 1, 0, 0 );
	glTranslatef( -300, 0, 0 );

	glColor3f( 1, 1, 1 );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'H' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'e' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'o' );

	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'W' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'o' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'r' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'd' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, '!' );

	glutSwapBuffers();
}
