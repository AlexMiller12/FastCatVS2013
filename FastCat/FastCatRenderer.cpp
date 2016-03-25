
#include "FastCatRenderer.h"
#include "Dependencies\maya\maya\MGlobal.h";
#include "ShaderHelper.h"
#include "CommonInclude.h"

//
//GLuint vbo = 0;
//GLuint vao = 0;
//GLuint shader_programme, vs, fs;
//
//float points[] = {
//	0.0f, 0.5f, 0.0f,
//	0.5f, -0.5f, 0.0f,
//	-0.5f, -0.5f, 0.0f
//};
//
//const char* vertex_shader =
//"#version 400\n"
//"in vec3 vp;"
//"void main () {"
//"  gl_Position = vec4 (vp, 1.0);"
//"}";
//
//const char* fragment_shader =
//"#version 400\n"
//"out vec4 frag_colour;"
//"void main () {"
//"  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
//"}";

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

FastCatRenderer::FastCatRenderer() : isReady(false)
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


void FastCatRenderer::init()
{
	createWindow();

	/*std::string vsFileName( SHADER_DIR );
	std::string psFileName( SHADER_DIR );
	vsFileName += "/test_vs.glsl";
	psFileName += "/test_ps.glsl";

	std::vector<GLenum> types;
	types.push_back( GL_VERTEX_SHADER );
	types.push_back( GL_FRAGMENT_SHADER );
	std::vector<const char *> fileNames;
	fileNames.push_back( vsFileName.c_str() );
	fileNames.push_back( psFileName.c_str() );
	ShaderHelper::createProgramWithShaders( types, fileNames, shader_programme );*/
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
	testMesh->adaptiveCCAllLevels();

	//// TODO: Temp?
	//glEnable( GL_DEPTH_TEST ); // enable depth-testing
	//glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"

	//// wipe the drawing surface clear
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	////glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram( shader_programme );

	//testMesh->bindDebugBuffers(3);

	//glm::mat4 MVP = camera->proj * camera->view; // model matrix is identity
	//GLint loc = ShaderHelper::getUniformLocation(shader_programme, "MVP");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
	//
	//glDrawArrays(GL_TRIANGLES, 0, testMesh->getNumVerticesDebug(3));

	////glBindVertexArray( vao );
	//// draw points 0-3 from the currently bound VAO with current in-use shader
	////glDrawArrays( GL_TRIANGLES, 0, 3 );
	//// update other events like input handling 
	//glfwPollEvents();
	//// put the stuff we've been drawing onto the display
	//glfwSwapBuffers( window );

	//glBindVertexArray(0); // unbind
	//testMesh->clearDebugBuffers();
}
