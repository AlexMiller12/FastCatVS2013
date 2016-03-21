#ifndef QUAD_TESS_PROG
#define QUAD_TESS_PROG

#include "BasicShaders.h"
#include "ShaderProgram.h"

class QuadTessellatorProgram : public ShaderProgram
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
		
//--------------------------------------------------------------------------METHODS:

public:
	void draw( mat4 modelView, mat4 projection );
	bool init();
	void updateControlPoints( vector<GLfloat> controlPoints );

};

#endif