#include "FullPatchProgram.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

//--------------------------------------------------------------------------METHODS:

void FullPatchProgram::draw( mat4 modelView, mat4 projection )
{
	use();
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	enableVec4Attribute( "in_position" );

	//setUniform( "u_modelView", modelView );
	setUniform( "u_mvp", projection * modelView );
	//setUniform( "u_modelRotation", mat3( 1.0 ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferHandle );

	glPatchParameteri( GL_PATCH_VERTICES, 16 );

	glDrawElements( GL_PATCHES,
					numIndices,
					GL_UNSIGNED_INT,
					0 );

	GLUtil::printErrors();
}

bool FullPatchProgram::init()
{
	// Initialize program without index buffer and load shaders
	if( ! ShaderProgram::init( true ) || ! loadShaders() )
	{
		return false;
	}

	createVBO( "in_position", ShaderProgram::gl_Vertex );
	
	if( ! finalizeProgram() )
	{
		return false;
	}

	use();
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	glClearColor( 0.7f, 0.6f, 0.5f, 1.0f );

	return true;
}

bool FullPatchProgram::init( GLuint sharedVAO, GLuint sharedVBO )
{
	// Initialize program without index buffer and load shaders
	if( ! ShaderProgram::init( sharedVAO, true ) || ! loadShaders() )
	{
		return false;
	}

	shareExistingVBO( "in_position", ShaderProgram::gl_Vertex, sharedVBO );

	if( ! finalizeProgram() )
	{
		return false;
	}
	use();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glClearColor( 0.7f, 0.6f, 0.5f, 1.0f );

	return true;
}

void FullPatchProgram::updateControlPoints( vector<GLfloat> newControlPoints )
{
	use();
	numVertices = newControlPoints.size();
	setVBO( "in_position", newControlPoints );
}

//--------------------------------------------------------------------------HELPERS:

bool FullPatchProgram::loadShaders()
{
	string vertSource, contSource, evalSource, geomSource, fragSource;
	string directory = IOUtil::executionPath() + "\\Shaders\\";

	if( ! IOUtil::readWholeFile( directory + "no_mvp.vert", vertSource ) ||
		! attachShader( vertSource, GL_VERTEX_SHADER ) )
	{
		return false;
	}
	if( ! IOUtil::readWholeFile( directory + "bspline.cont", contSource ) ||
		! attachShader( contSource, GL_TESS_CONTROL_SHADER ) )
	{
		return false;
	}
	if( ! IOUtil::readWholeFile( directory + "bspline.eval", evalSource ) ||
		! attachShader( evalSource, GL_TESS_EVALUATION_SHADER ) )
	{
		return false;
	}
	if( ! IOUtil::readWholeFile( directory + "basic.geom", geomSource ) ||
		! attachShader( geomSource, GL_GEOMETRY_SHADER ) )
	{
		return false;
	}
	if( ! IOUtil::readWholeFile( directory + "basic_lighting.frag", fragSource ) ||
		! attachShader( fragSource, GL_FRAGMENT_SHADER ) )
	{
		return false;
	}
	return true;
}