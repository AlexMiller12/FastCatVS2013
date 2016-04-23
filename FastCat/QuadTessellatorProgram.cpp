#include "QuadTessellatorProgram.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

//--------------------------------------------------------------------------METHODS:

void QuadTessellatorProgram::draw( mat4 modelView, mat4 projection )
{
	use();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	enableVec3Attribute( "Position" );

	setUniform( "Projection", projection );
	setUniform( "Modelview", modelView );
	setUniform( "NormalMatrix", mat3( 1.0f ) );
	//setUniform( "NormalMatrix", mat3( modelView ) );
	GLUtil::printErrors();
	glPatchParameteri( GL_PATCH_VERTICES, 16 );

	glDrawElements( GL_PATCHES, 
					numIndices * sizeof( GLushort ), 
					GL_UNSIGNED_SHORT, 
					0 );

	GLUtil::printErrors();
}

bool QuadTessellatorProgram::init()
{
	// Initialize program without index buffer
	if( ! ShaderProgram::init( true ) )
	{
		return false;
	}
	if( ! attachShader( BasicShaders::noMVP_v, GL_VERTEX_SHADER )			 ||
		! attachShader( BasicShaders::simple_tc, GL_TESS_CONTROL_SHADER )	 ||
		! attachShader( BasicShaders::simple_te, GL_TESS_EVALUATION_SHADER ) ||
		//! attachShader( BasicShaders::flat_te, GL_TESS_EVALUATION_SHADER ) ||
		! attachShader( BasicShaders::simple_g, GL_GEOMETRY_SHADER )		 ||
		! attachShader( BasicShaders::lambert_f, GL_FRAGMENT_SHADER ) )
	{ 
		return false;
	}

	createVBO( "Position", ShaderProgram::gl_Vertex );

	if( ! finalizeProgram() ) 	return false;

		
	mat4 bezierBasisFunctions = mat4( -1,  3, -3,  1, 
								       3, -6,  3,  0, 
									  -3,  3,  0,  0, 
									   1,  0,  0,  0 );

	setUniform( "B", bezierBasisFunctions );
	setUniform( "BT", glm::transpose( bezierBasisFunctions ) );

	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.7f, 0.6f, 0.5f, 1.0f );

	return true;
}

void QuadTessellatorProgram::updateControlPoints( vector<GLfloat> newControlPoints )
{
	numVertices = newControlPoints.size();
	setVec3VBO( "Position", newControlPoints );
}

//--------------------------------------------------------------------------HELPERS:
