//#include "FastCatRenderer.h"

#include <stdio.h>  
#include <stdlib.h>  

// For IO:
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"

#include "Renderer.h"
#include "WireFrameProgram.h"
#include "QuadTessellatorProgram.h"
#include "AlexCamera.h"
#include "gumbo.h"

//--------------------------------------------------------------------------GLOBALS:

// 8 points, 24 elements
GLfloat cube_vertices[] = {
	// front
	-10.0, -10.0, 10.0,
	10.0, -10.0, 10.0,
	10.0, 10.0, 10.0,
	-10.0, 10.0, 10.0,
	// back
	-10.0, -10.0, -10.0,
	10.0, -10.0, -10.0,
	10.0, 10.0, -10.0,
	-10.0, 10.0, -10.0,
};
// 8 points, 24 elements
GLfloat cube_colors[] = {
	// front colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
	// back colors
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0,
	1.0, 1.0, 1.0,
};
// 12 faces, 36 elements
GLushort cube_elements[] = {
	// front
	0, 1, 2,
	2, 3, 0,
	// top
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// bottom
	4, 0, 3,
	3, 7, 4,
	// left
	4, 5, 1,
	1, 0, 4,
	// right
	3, 2, 6,
	6, 7, 3,
};

Renderer renderer;
AlexCamera camera;
WireFrameProgram wireFramProgram;
QuadTessellatorProgram quadProgram;
std::vector<GLfloat> vertices, colors;
std::vector<GLushort> indices;

//-----------------------------------------------------------------------PROTOTYPES:

vector<GLfloat> makeGumbo();
void setupCamera();
void showCube();
void showGumbo();

//-----------------------------------------------------------------------------MAIN:

int main( int numArguments, char** arguments )
{
	//showCube();
	showGumbo();
	return 0;
}

//------------------------------------------------------------------------FUNCTIONS:

vector<GLushort> makeGumboIndices()
{
	int numVertices = sizeof( PatchData ) / ( sizeof( float ) * 3 );
	vector<GLushort> indices;
	for( int i = 0; i < numVertices; i++ )
	{
		indices.push_back( i );
	}
	return indices;
}

vector<GLfloat> makeGumbo()
{
	int numVertices = sizeof( PatchData ) / ( sizeof( float ) * 3 );
	vector<GLfloat> controlPoints;
	for( int vert = 0; vert < numVertices; vert++ )
	{
		controlPoints.push_back( PatchData[vert][0] ); // x
		controlPoints.push_back( PatchData[vert][1] ); // y
		controlPoints.push_back( PatchData[vert][2] ); // z
	}
	return controlPoints;
}

void showCube()
{
	// We will get our data in vectors probably
	vertices.assign( cube_vertices, cube_vertices + 24 );
	colors.assign( cube_colors, cube_colors + 24 );
	indices.assign( cube_elements, cube_elements + 36 );

	// Init GL
	renderer.createWindow();
	renderer.bind();

	// Set up camera so we can get our projection matrix
	setupCamera();

	if( ! wireFramProgram.init() )
	{
		exit( 1 );
	}

	while( ! renderer.shouldClose() )
	{
		wireFramProgram.use();
		// Pretend that these are changing each frame
		wireFramProgram.updateVertexPositions( vertices );
		wireFramProgram.updateVertexColors( colors );
		wireFramProgram.setIndices( indices );
		// Run program
		wireFramProgram.draw( camera.viewProjectionMatrix() );
		// Display the framebuffer to which we just wrote
		renderer.swapBuffers();
	}
	renderer.unbind();
	renderer.closeWindow();
}

void showGumbo()
{
	// Init GL
	renderer.createWindow();
	renderer.bind();

	// Set up camera so we can get our projection matrix
	setupCamera();
	
	if( ! quadProgram.init() )
	{
		printf( "blargblarg" );
		exit( 1 );
	}
	

	vector<GLfloat> gumboControlPoints = makeGumbo();
	vector<GLushort> gumboIndices = makeGumboIndices();

	quadProgram.use();
	quadProgram.updateControlPoints( gumboControlPoints );
	quadProgram.setUniform( "AmbientMaterial", vec3( 0.04f ) );
	quadProgram.setUniform( "LightPosition", vec3( 0.25f, 0.25f, 1.0f ) );
	quadProgram.setUniform( "AmbientMaterial", vec3( 0.04f ) );
	quadProgram.setUniform( "SpecularMaterial", vec3( 0.5f ) );
	quadProgram.setUniform( "DiffuseMaterial", vec3( 0, 0.75f, 0.75f ) );
	quadProgram.setUniform( "Shininess", 50.0f );
	quadProgram.setUniform( "AmbientMaterial", vec3() );
	quadProgram.setUniform( "TessLevelInner", 6 );
	quadProgram.setUniform( "TessLevelOuter", 6 );

	
	while( ! renderer.shouldClose() )
	{
		quadProgram.use();
		// Pretend that these are changing each frame
		quadProgram.updateControlPoints( gumboControlPoints );
		quadProgram.setIndices( gumboIndices );
		// Run program
		quadProgram.draw( camera.viewMatrix(), camera.projectionMatrix() );
		// Display the framebuffer to which we just wrote
		renderer.swapBuffers();
	}
	renderer.unbind();
	renderer.closeWindow();
}

void setupCamera()
{
	int screen_width = 640, screen_height = 480;
	float nearclip = 1.0f, farClip = 200.0f;
	camera = AlexCamera( 45.0f, screen_width, screen_height, nearclip, farClip );
	vec3 camPos = vec3( 35, -35, 45 );
	vec3 lookAt = vec3( 0 );
	camera.lookAt( camPos, lookAt );
}

bool readWholeFile( const char *fileName, std::string &ret_content )
{
	bool success = true;
	std::ifstream ifs( fileName, std::ifstream::in | std::ifstream::binary );

	if( ifs )
	{
		ifs.seekg( 0, std::ifstream::end );
		int length = ifs.tellg();
		ifs.seekg( 0, std::ifstream::beg );

		ret_content.resize( length );
		ifs.read( &ret_content[0], length );

		ifs.close();
	}
	else
	{
		std::cout << "Cannot open shader file: " << fileName << '\n';
		return !success;
	}

	return success;
}
