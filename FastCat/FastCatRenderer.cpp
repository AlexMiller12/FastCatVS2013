
#include "FastCatRenderer.h"
#include "Dependencies\maya\maya\MGlobal.h";
#include "ShaderHelper.h"
#include "CommonInclude.h"


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

std::shared_ptr<Camera> pCam = nullptr;

FastCatRenderer::FastCatRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
	: isReady(false), baseTessFactor(btf), controlMesh(cm), camera(c),
	perFrameBufferGenerated(false), perLevelBufferGenerated(false)
{
	state = FastCatStates::NEUTRAL;

	pCam = c;
	fullPatchNoSharpRenderer = std::make_shared<FullPatchNoSharpRenderer>(btf, cm, c);
}

FastCatRenderer::~FastCatRenderer()
{
	pCam = nullptr;
}

//------------------------------------------------------------------------FUNCTIONS:

//Define an error callback  
void error_callback( int error, const char* description )
{
	fputs( description, stderr );
	_fgetchar();
}

//Define the key input callback  
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	else if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		pCam->moveForward();
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		pCam->moveBackward();
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		pCam->moveLeft();
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		pCam->moveRight();
	}
	else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		pCam->moveDown();
	}
	else if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		pCam->moveUp();
	}
	else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		pCam->rotateLeft();
	}
	else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		pCam->rotateRight();
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		pCam->rotateUp();
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		pCam->rotateDown();
	}
}

//--------------------------------------------------------------------------METHODS:

void FastCatRenderer::closeWindow()
{
	MGlobal::displayInfo( "closeWindow" );

	ShaderHelper::cleanup();

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow( window );
	//Finalize and clean up GLFW  
	glfwTerminate();
}

void FastCatRenderer::createWindow()
{
	if( window != NULL )
	{
		//MGlobal::displayInfo( "FastCatRenderer -- Window already created" );
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
	window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "Test Window", NULL, NULL );
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
	glewExperimental = true;
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

	std::string vsFileName(SHADER_DIR);
	std::string psFileName(SHADER_DIR);
	vsFileName += "/test_vs.glsl";
	psFileName += "/test_ps.glsl";

	std::vector<GLenum> types;
	types.push_back(GL_VERTEX_SHADER);
	types.push_back(GL_FRAGMENT_SHADER);
	std::vector<const char *> fileNames;
	fileNames.push_back(vsFileName.c_str());
	fileNames.push_back(psFileName.c_str());
	ShaderHelper::createProgramWithShaders(types, fileNames, shader_programme);

	fullPatchNoSharpRenderer->createShaderProgram();

	//GLuint vs = glCreateShader( GL_VERTEX_SHADER );
	//glShaderSource( vs, 1, &vertex_shader, NULL );
	//glCompileShader( vs );
	//GLuint fs = glCreateShader( GL_FRAGMENT_SHADER );
	//glShaderSource( fs, 1, &fragment_shader, NULL );
	//glCompileShader( fs );

	//shader_programme = glCreateProgram();
	//glAttachShader( shader_programme, fs );
	//glAttachShader( shader_programme, vs );
	//glLinkProgram( shader_programme );

	//glGenBuffers( 1, &vbo );
	//glBindBuffer( GL_ARRAY_BUFFER, vbo );
	//glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), points, GL_STATIC_DRAW );
	
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glBindVertexArray(0); // unbind
	//glEnableVertexAttribArray( 0 );
	//glBindBuffer( GL_ARRAY_BUFFER, vbo );
	//glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
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
	//if( i++ > 0 )
	//{
	//	closeWindow();
	//	return;
	//}
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
	static bool onceThrough = false;

	if (!onceThrough)
	{
		controlMesh->adaptiveCCAllLevels();
		fullPatchNoSharpRenderer->generateIndexBuffer();

		glEnable(GL_DEPTH_TEST); // enable depth-testing
		glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		onceThrough = true;
	}

	// wipe the drawing surface clear
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glUseProgram( shader_programme );

	//testMesh->bindDebugBuffers(3);

	//glm::mat4 MVP = camera->proj * camera->view; // model matrix is identity
	//GLint loc = ShaderHelper::getUniformLocation(shader_programme, "MVP");
	//glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);
	
	//glDrawArrays(GL_TRIANGLES, 0, testMesh->getNumVerticesDebug(3));

	//glBindVertexArray( vao );
	// draw points 0-3 from the currently bound VAO with current in-use shader
	//glDrawArrays( GL_TRIANGLES, 0, 3 );

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, controlMesh->vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	setPerFrameUniformBlock(glm::vec3(1.f, 1.f, 1.f), camera->farClip - camera->nearClip);

	int numLevels = controlMesh->levels.size();
	float tessFactor = fmax(1.0f, baseTessFactor);
	float tessFactorNextLevel = fmax(1.0f, tessFactor / 2.0f);

	for (int i = 0; i < numLevels; ++i)
	{
		float maxTessFactor = fmin(64.0f, fmax(1.0, pow(2.0f, controlMesh->maxSubdivisionLevel - i)));

		setPerLevelUniformBlock(tessFactor, tessFactorNextLevel, maxTessFactor, i,
			controlMesh->levels[i]->firstVertexOffset, glm::vec4(1.f, 1.f, 1.f, 1.f));
		
		fullPatchNoSharpRenderer->renderLevel(i);

		tessFactor = fmax(1.0f, tessFactor / 2.0f);
		tessFactorNextLevel = fmax(1.0f, tessFactor / 2.0f);
	}

	glBindVertexArray(0); // unbind
	//testMesh->clearDebugBuffers();
}


void FastCatRenderer::setPerFrameUniformBlock(const glm::vec3 &lightDir, float znzf)
{
	if (!perFrameBufferGenerated)
	{
		glGenBuffers(1, &perFrameBufferName);
		glBindBuffer(GL_UNIFORM_BUFFER, perFrameBufferName);
		glBufferData(GL_UNIFORM_BUFFER, 276, NULL, GL_STREAM_DRAW);
		perFrameBufferGenerated = true;
	}

	char buff[276] = { 0 };
	glm::mat4 worldMatrix(1.0);
	glm::mat4 mvp = camera->proj * camera->view;

	memcpy(buff, glm::value_ptr(worldMatrix), 16 * sizeof(float));
	memcpy(buff + 16 * sizeof(float), glm::value_ptr(camera->view), 16 * sizeof(float));
	memcpy(buff + 32 * sizeof(float), glm::value_ptr(camera->proj), 16 * sizeof(float));
	memcpy(buff + 48 * sizeof(float), glm::value_ptr(mvp), 16 * sizeof(float));
	memcpy(buff + 64 * sizeof(float), glm::value_ptr(lightDir), 3 * sizeof(float));
	*((float *)(buff + 68 * sizeof(float))) = znzf;

	glBindBuffer(GL_UNIFORM_BUFFER, perFrameBufferName);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 276, buff);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, perFrameBufferName);
}


void FastCatRenderer::setPerLevelUniformBlock(float tessfactor, float tessfactorNextLevel, float maxTessfactor,
	int level, int firstVertexOffset, const glm::vec4 &color)
{
	if (!perLevelBufferGenerated)
	{
		glGenBuffers(1, &perLevelBufferName);
		glBindBuffer(GL_UNIFORM_BUFFER, perLevelBufferName);
		glBufferData(GL_UNIFORM_BUFFER, 48, NULL, GL_STREAM_DRAW);
		perLevelBufferGenerated = true;
	}

	char buff[48] = { 0 };
	*((float *)buff) = tessfactor;
	*((float *)(buff + 4)) = tessfactorNextLevel;
	*((float *)(buff + 8)) = maxTessfactor;
	*((int *)(buff + 12)) = level;
	*((int *)(buff + 16)) = firstVertexOffset;
	memcpy(buff + 32, glm::value_ptr(color), 4 * sizeof(float));

	glBindBuffer(GL_UNIFORM_BUFFER, perLevelBufferName);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 48, buff);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, perLevelBufferName);
}