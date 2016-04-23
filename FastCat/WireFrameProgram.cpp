#include "WireFrameProgram.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

void WireFrameProgram::draw( mat4 mvp )
{
	use();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Enable pointers so we can access attribute in shader
	enableVec3Attribute( "in_position" );
	enableVec3Attribute( "in_color" );

	// Pass in uniform mvp (setting it each frame prettending it changes)
	setUniform( "mvp", mvp );
	GLUtil::printErrors();
	// Draw wireframe
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// Draw call
	glDrawElements( GL_TRIANGLES, 
					numIndices * sizeof( GLushort ), 
					GL_UNSIGNED_SHORT, 
					0 );
	GLUtil::printErrors();
}

bool WireFrameProgram::init()
{
	// Initialize program with index buffer
	if( ! ShaderProgram::init( true ) )
	{
		return false;
	}

	if( ! attachShader( vertex_shader, GL_VERTEX_SHADER )  ||
		! attachShader( fragment_shader, GL_FRAGMENT_SHADER ) )
	{
		return false;
	}

	createVBO( "in_position", ShaderProgram::gl_Vertex );
	createVBO( "in_color", ShaderProgram::gl_Color );

	if( ! finalizeProgram() )
	{
		return false;
	}

	// Set some GL state stuff that will be saved to our program's vao
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LESS );

	return true;
}

void WireFrameProgram::updateVertexPositions( vector<GLfloat> vertexPositions )
{
	setVec3VBO( "in_position", vertexPositions );
}

void WireFrameProgram::updateVertexColors( vector<GLfloat> vertexColors )
{
	setVec3VBO( "in_color", vertexColors );
}

//--------------------------------------------------------------------------HELPERS:
