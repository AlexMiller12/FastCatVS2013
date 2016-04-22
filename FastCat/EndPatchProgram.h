#ifndef END_PATCH_PROGRAM
#define END_PATCH_PROGRAM

#include "ShaderProgram.h"
#include "ShaderProgram.h"
#include "IOUtil.h"

using namespace std;

class EndPatchProgram : public ShaderProgram
{
	
//--------------------------------------------------------------------------STRUCTS:


//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:
	
private:
	int numVertices;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
		
//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

public:
	void draw( mat4 mvp );
	bool init();
	void updateControlPoints( vector<GLfloat> newControlPoints );

private:
	bool loadShaders();

};

#endif