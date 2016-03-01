
#include "FastCatRenderer.h"
#include "Dependencies\maya\maya\MGlobal.h";


GLuint vbo = 0;
GLuint vao = 0;
GLuint shader_programme, vs, fs;

float points[] = {
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f
};

const char* vertex_shader =
"#version 400\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

const char* fragment_shader =
"#version 400\n"
"out vec4 frag_colour;"
"void main () {"
"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
"}";

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
void FastCatRenderer::init()
{

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent( window );

	GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vs, 1, &vertex_shader, NULL );
	glCompileShader( vs );
	GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fs, 1, &fragment_shader, NULL );
	glCompileShader( fs );

	shader_programme = glCreateProgram();
	glAttachShader( shader_programme, fs );
	glAttachShader( shader_programme, vs );
	glLinkProgram( shader_programme );

	glGenBuffers( 1, &vbo );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), points, GL_STATIC_DRAW );
	
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
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
	if( i++ > 0 )
	{
		closeWindow();
		return;
	}
	testPass();
	//if( i == 0 )   state = FastCatStates::NEUTRAL;
	//else if( i == 1 )   state = FastCatStates::TESTING;
	//else if( i++ == 2 )   closeWindow();
	//render();

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

	// TODO: Temp?
	glEnable( GL_DEPTH_TEST ); // enable depth-testing
	glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"

	// wipe the drawing surface clear
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram( shader_programme );
	glBindVertexArray( vao );
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays( GL_TRIANGLES, 0, 3 );
	// update other events like input handling 
	glfwPollEvents();
	// put the stuff we've been drawing onto the display
	glfwSwapBuffers( window );
}
