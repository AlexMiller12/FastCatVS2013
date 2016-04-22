#include "EndPatchProgram.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

void EndPatchProgram::draw( mat4 mvp )
{
	use();

	enableVec3Attribute( "in_position" );
	enableVec1Attribute( "in_vertexID" );
	//enableInt1Attribute( "in_vertexID" );

	glDrawElements( GL_TRIANGLES,
					numIndices * sizeof( GLuint ),
					GL_UNSIGNED_INT,
					0 );

	GLUtil::printErrors();
}

bool EndPatchProgram::init()
{
	if( ! ShaderProgram::init( true ) ||  ! loadShaders() )
	{
		return false;
	}

	createVBO( "in_position", ShaderProgram::gl_Vertex );
	createVBO( "in_vertexID", ShaderProgram::gl_MultiTexCoord7 );

	if( ! finalizeProgram() )
	{
		return false;
	}

	use();

	ShaderProgram::createSBO( "valenceBuffer" );
	ShaderProgram::createSBO( "neighborIndexBuffer" );
	ShaderProgram::createSBO( "vertexData" );

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable( GL_DEPTH_TEST ); // enable depth-testing
	glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"

	return true;
}

//--------------------------------------------------------------------------HELPERS:

bool EndPatchProgram::loadShaders()
{
	string vertSource, contSource, evalSource, geomSource, fragSource;
	string directory = IOUtil::executionPath() + "\\Shaders\\";

	if( ! IOUtil::readWholeFile( directory + "end_patch.vert", vertSource ) ||
		! attachShader( vertSource, GL_VERTEX_SHADER ) )
	{
		return false;
	}
	if( ! IOUtil::readWholeFile( directory + "end_patch.frag", fragSource ) ||
		! attachShader( fragSource, GL_FRAGMENT_SHADER ) )
	{
		return false;
	}
	return true;
}