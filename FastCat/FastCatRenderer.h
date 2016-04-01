#ifndef FC_RENDERER
#define FC_RENDERER

//Include GLEW  
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include <memory>
#include <stdio.h>  
#include <stdlib.h>

#include "ControlMesh.h"
#include "Camera.h"
#include "PatchRenderer.h"

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
	bool isReady;
	bool meshSubdivided;

	GLuint vao;
	GLuint shader_programme;

	float baseTessFactor;
	std::shared_ptr<ControlMesh> controlMesh;
	std::shared_ptr<Camera> camera;

	std::shared_ptr<FullPatchNoSharpRenderer> fullPatchNoSharpRenderer;
	std::shared_ptr<EndPatchRenderer> endPatchRenderer;

	bool perFrameBufferGenerated;
	GLuint perFrameBufferName;

	bool perLevelBufferGenerated;
	GLuint perLevelBufferName;

	GLFWwindow* window;
	FastCatStates state;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	FastCatRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c);
	virtual ~FastCatRenderer();
	
//--------------------------------------------------------------------------METHODS:

public:
	void closeWindow();
	void createWindow();
	void init();

	void render(); //TODO will be private if loop lives in this class
	bool shouldWindowClose();
	void test();

	// @lightDir should point to the light
	// @znzf is the distance between near and far clips
	virtual void setPerFrameUniformBlock(const glm::vec3 &lightDir, float znzf);

	virtual void setPerLevelUniformBlock(float tessfactor, float tessfactorNextLevel, float maxTessfactor,
		int level, int firstVertexOffset, const glm::vec4 &color);

private:
	void testPass();
};

#endif