#ifndef FULL_PATCH_PROG
#define FULL_PATCH_PROG

#include "BasicShaders.h"
#include "ShaderProgram.h"
#include "IOUtil.h"

using namespace std;

class FullPatchProgram : public ShaderProgram
{
	
//----------------------------------------------------------------------------ENUMS:
	
//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:

private:
	float innerTessellationLevel;
	float outerTessellationLevel;
	int numVertices;
	int controlPointsPerPatch;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
		
//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

public:
	void draw( mat4 modelView, mat4 projection );
	bool init();
	//bool init( GLuint sharedVBO );
	bool init( GLuint sharedVAO, GLuint sharedVBO );
	void updateControlPoints( vector<GLfloat> controlPoints );

private:
	bool loadShaders();
};

#endif