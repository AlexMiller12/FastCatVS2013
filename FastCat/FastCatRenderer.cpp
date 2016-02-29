
#include "FastCatRenderer.h"
#include "Dependencies\maya\maya\MGlobal.h";

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

FastCatRenderer::FastCatRenderer()
{
	state = FastCatStates::NEUTRAL;
}

FastCatRenderer::~FastCatRenderer()
{}

//------------------------------------------------------------------------FUNCTIONS:

//Define an error callback  
void error_callback( int error, const char* description )
{
	fputs( description, stderr );
	_fgetchar();
}

//Define the key input callback  
void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GL_TRUE );
}

//--------------------------------------------------------------------------METHODS:

void FastCatRenderer::closeWindow()
{
	MGlobal::displayInfo( "closeWindow" );

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow( window );
	//Finalize and clean up GLFW  
	glfwTerminate();
}

void FastCatRenderer::createWindow()
{
	if( window != NULL )
	{
		MGlobal::displayInfo( "FastCatRenderer -- Window already created" );
		return;
	}
	//Set the error callback  
	glfwSetErrorCallback( error_callback );

	//Initialize GLFW  
	if( ! glfwInit() )
	{
		return;
	}
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
}

// Returns true if the ESC key had been pressed or if the window had been closed  
bool FastCatRenderer::shouldWindowClose()
{
	return window != NULL  &&  glfwWindowShouldClose( window );
}

//TODO: temp
int i = 0;
void FastCatRenderer::test()
{
	if( i == 0 )   state = FastCatStates::NEUTRAL;
	else if( i == 1 )   state = FastCatStates::TESTING;
	else if( i++ == 2 )   closeWindow();
	
	render();
	// Set render state
	//state = FastCatStates::TESTING;

	// Close window and clean up
	//closeWindow();

}

//--------------------------------------------------------------------------HELPERS:

void FastCatRenderer::render()
{
	// Check that window has been initialized
	if( window == NULL )
	{
		MGlobal::displayInfo( "FastCatRenderer -- Cannot render withuot window" );
		return;
	}
	// Bind to the window's context
	glfwMakeContextCurrent( window );
	// Perform GPU passes based on state
	switch( state )
	{
	case FastCatStates::NEUTRAL:
		// Just draw a blank screen
		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		break;
	case FastCatStates::TESTING:
		testPass();
		break;
	default:
		MGlobal::displayInfo( "FastCatRenderer -- Unrecognized state" );
	}
	//Swap buffers  
	glfwSwapBuffers( window );
	//Get and organize events, like keyboard and mouse input, window resizing, etc...  
	glfwPollEvents();
	// Unbind the window's context
	glfwMakeContextCurrent( NULL );
}

// Function to render hello world (to be passed to glut)
void FastCatRenderer::testPass()
{
	//Set a background color  
	glClearColor( 0.0f, 0.0f, 1.0f, 0.0f );

	//Clear color buffer  
	glClear( GL_COLOR_BUFFER_BIT );
}
