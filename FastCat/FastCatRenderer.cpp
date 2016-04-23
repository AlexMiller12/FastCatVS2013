
#include "FastCatRenderer.h"
#include "Dependencies\maya\maya\MGlobal.h";
#include "ShaderHelper.h"
#include "CommonInclude.h"


//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

#ifdef FAST_CAT_DEBUG_MODE
static int g_debugShowLevel = 0;
#endif

std::shared_ptr<Camera> pCam = nullptr;
static bool g_lmbPressed = false;
static glm::mat4 g_worldMatrix = glm::mat4(1.f);
static double mouseX = -1.0;
static double mouseY = -1.0;


FastCatRenderer::FastCatRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
	: isReady(false), meshSubdivided(false), baseTessFactor(btf), controlMesh(cm), camera(c),
	perFrameBufferGenerated(false), perLevelBufferGenerated(false)
{
	state = FastCatStates::NEUTRAL;

	fullPatchNoSharpRenderer = std::make_shared<FullPatchNoSharpRenderer>(btf, cm, c);
	fullPatchSharpRenderer = std::make_shared<FullPatchSharpRenderer>(btf, cm, c);
	partialPatchNoSharpRenderer = std::make_shared<PartialPatchNoSharpRenderer>(btf, cm, c);
	partialPatchSharpRenderer = std::make_shared<PartialPatchSharpRenderer>(btf, cm, c);

	endPatchRenderer = std::make_shared<EndPatchRenderer>(btf, cm, c);

	createWindow();
	init();
	isReady = true;

	// Initialize global variables
	pCam = c;
	g_lmbPressed = false;
	g_worldMatrix = glm::mat4(1.f);
	mouseX = -1.0;
	mouseY = -1.0;

#ifdef FAST_CAT_DEBUG_MODE
	g_debugShowLevel = 0;
#endif
}

FastCatRenderer::~FastCatRenderer()
{
	// Reset global variables
	pCam = nullptr;
	g_lmbPressed = false;
	g_worldMatrix = glm::mat4(1.f);
	mouseX = -1.0;
	mouseY = -1.0;

#ifdef FAST_CAT_DEBUG_MODE
	g_debugShowLevel = 0;
#endif
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
#ifdef FAST_CAT_DEBUG_MODE
	else if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		if (g_debugShowLevel < 6)
		{
			++g_debugShowLevel;
		}
	}
	else if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		if (g_debugShowLevel > 0)
		{
			--g_debugShowLevel;
		}
	}
#endif
}


void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	const float kSensitivity = 0.2;

	if (g_lmbPressed)
	{
		float dx = xpos - mouseX;
		float dy = ypos - mouseY;

		g_worldMatrix = glm::rotate(kSensitivity * dy, glm::vec3(1.f, 0.f, 0.f)) *
			glm::rotate(kSensitivity * dx, glm::vec3(0.f, 1.f, 0.f)) *
			g_worldMatrix;

		mouseX = xpos;
		mouseY = ypos;
	}
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &mouseX, &mouseY);
		g_lmbPressed = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		g_lmbPressed = false;
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
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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

#ifdef FAST_CAT_DEBUG_MODE
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
#else
	fullPatchNoSharpRenderer->createShaderProgram();
	fullPatchSharpRenderer->createShaderProgram();
	partialPatchNoSharpRenderer->createShaderProgram();
	partialPatchSharpRenderer->createShaderProgram();
	endPatchRenderer->createShaderProgram();
#endif
	
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );
	glBindVertexArray(0); // unbind
}

// Returns true if the ESC key had been pressed or if the window had been closed  
bool FastCatRenderer::shouldWindowClose()
{
	return window != NULL  &&  glfwWindowShouldClose( window );
}


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
	if (!meshSubdivided)
	{
		controlMesh->adaptiveCCAllLevels();

#ifndef FAST_CAT_DEBUG_MODE
		fullPatchNoSharpRenderer->generateIndexBuffer();
		fullPatchSharpRenderer->generateIndexBuffer();
		partialPatchNoSharpRenderer->generateIndexBuffer();
		partialPatchSharpRenderer->generateIndexBuffer();
		endPatchRenderer->generateIndexBuffer();
#endif

		glEnable(GL_DEPTH_TEST); // enable depth-testing
		glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);

		meshSubdivided = true;
	}

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

#ifdef FAST_CAT_DEBUG_MODE
	// draw debug info
	glUseProgram(shader_programme);
	glm::mat4 MVP = camera->proj * camera->view * g_worldMatrix; // model matrix is identity
	GLint loc = ShaderHelper::getUniformLocation(shader_programme, "MVP");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &MVP[0][0]);

	int debugShowLevel = (g_debugShowLevel > controlMesh->maxSubdivisionLevel) ? controlMesh->maxSubdivisionLevel : g_debugShowLevel;
	controlMesh->bindDebugBuffers(debugShowLevel);
	glDrawArrays(GL_TRIANGLES, 0, controlMesh->getNumVerticesDebug(debugShowLevel));
#else
	// Draw patches
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, controlMesh->vbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	setPerFrameUniformBlock(glm::vec3(1.f, 1.f, 1.f), camera->farClip - camera->nearClip);

	int numLevels = controlMesh->levels.size();
	float tessFactor = fmax(1.0f, baseTessFactor);
	float tessFactorNextLevel = fmax(1.0f, tessFactor / 2.0f);

	// used to change patch colors
	auto setDrawColor = [this](const glm::vec4 &newColor)
	{
		glNamedBufferSubData(perLevelBufferName, 32, 16, glm::value_ptr(newColor));
	};

	for (int i = 0; i < numLevels; ++i)
	{
		float maxTessFactor = fmin(64.0f, fmax(1.0, pow(2.0f, controlMesh->maxSubdivisionLevel - i)));

		setPerLevelUniformBlock(tessFactor, tessFactorNextLevel, maxTessFactor, i,
			controlMesh->levels[i]->firstVertexOffset, glm::vec4(1.f, 1.f, 1.f, 1.f));

		fullPatchNoSharpRenderer->renderLevel(i, setDrawColor);
		fullPatchSharpRenderer->renderLevel(i, setDrawColor);
		partialPatchNoSharpRenderer->renderLevel(i, setDrawColor);
		partialPatchSharpRenderer->renderLevel(i, setDrawColor);
		endPatchRenderer->renderLevel(i, setDrawColor);

		tessFactor = fmax(1.0f, tessFactor / 2.0f);
		tessFactorNextLevel = fmax(1.0f, tessFactor / 2.0f);
	}

	glBindVertexArray(0); // unbinds
#endif
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
	glm::mat4 mvp = camera->proj * camera->view * g_worldMatrix;

	memcpy(buff, glm::value_ptr(g_worldMatrix), 16 * sizeof(float));
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