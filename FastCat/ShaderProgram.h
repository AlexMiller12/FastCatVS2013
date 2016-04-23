#ifndef SHADER_PROGRAM
#define SHADER_PROGRAM

#include <unordered_map>
#include <vector>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "GLUtil.h"

using namespace std;
using glm::vec3; using glm::vec4; using glm::mat3; using glm::mat4;

class ShaderProgram
{
	
//----------------------------------------------------------------------------ENUMS:
	
//------------------------------------------------------------------------CONSTANTS:

public:
	// https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php
	// These constants are reserved in opengl (at least for NVidia): 
	const static GLint gl_Vertex			= 0;
	const static GLint gl_Normal			= 2;
	const static GLint gl_Color				= 3;
	const static GLint gl_SecondaryColor	= 4;
	const static GLint gl_FogCoord			= 5;
	const static GLint gl_MultiTexCoord0	= 8;
	const static GLint gl_MultiTexCoord1	= 9;
	const static GLint gl_MultiTexCoord2	= 10;
	const static GLint gl_MultiTexCoord3	= 11;
	const static GLint gl_MultiTexCoord4	= 12;
	const static GLint gl_MultiTexCoord5	= 13;
	const static GLint gl_MultiTexCoord6	= 14;
	const static GLint gl_MultiTexCoord7	= 15;
	
private:
	const static bool DEBUG = true;

//---------------------------------------------------------------------------FIELDS:

protected:
	// Program handle
	GLuint handle;
	// Handle to the vao to which we will account any vbos
	GLuint vertexArrayObjectHandle;
	GLuint indexBufferHandle;
	int numIndices;
	
private:
	static unordered_map<string, GLint> sboHandles;
	unordered_map<string, GLint> attributeIndices;
	unordered_map<string, GLint> attributeLocations;
	unordered_map<string, GLint> uniformLocations;
	vector<GLuint> shaders;
	unordered_map<string, GLint> sboBlockIndices;

	//unordered_map<string, GLint> samplerTextureNumbers;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

public:
	ShaderProgram();
	~ShaderProgram();

//--------------------------------------------------------------------------METHODS:

public:
	bool attachShader( const char* source, GLenum type );
	bool attachShader( string source, GLenum type );
	
	bool bindToVAO(); // TODO private? 
	static bool createSBO( string name );
	bool createVBO( string attributeName, GLuint attributeindex );
	bool enableInt1Attribute( string attributeName );
	bool enableVec1Attribute( string attributeName );
	bool enableVec3Attribute( string attributeName );
	bool enableVec4Attribute( string attributeName );
	bool enableFloatAttribute( string attributeName, int floatsPerVertex );
	bool enableIntAttribute( string attributeName, int intsPerVertex );

	bool finalizeProgram();
	GLuint getAttributeLocation( string name ); //TODO protected? 
	GLuint getBlockIndex( string name );
	static GLuint getSBOHandle( string name );
	GLuint getHandle();
	GLuint getUniformLocation( string name );  //TODO protected?

	bool init( bool createIndexBuffer );
	bool init( GLuint vaoHandle, bool createIndexBuffer );

	bool printProgramErrors();
	bool printShaderErrors();



	bool setIndices( GLuint indices[], 
					 int numFaces, 
					 GLenum usage = GL_STATIC_DRAW );
	bool setIndices( GLushort indices[],
					 int numFaces, 
					 GLenum usage = GL_STATIC_DRAW );
	bool setIndices( vector<GLuint> indices, GLenum usage = GL_STATIC_DRAW ); 
	bool setIndices( vector<GLushort> indices, GLenum usage = GL_STATIC_DRAW );
	
	static bool setSBO( string name,
						GLsizei size,
						const void* data,
						GLenum usage = GL_DYNAMIC_COPY );

	bool setSBOBindingPoint( GLuint bindingPointIndex, string sboName );
	bool setSBOBindingPoint( GLuint bindingPointIndex, string sboName, GLuint sboHandle );

	bool setUniform( string uniformName, mat3 value );
	bool setUniform( string uniformName, mat4 value );
	bool setUniform( string uniformName, vec3 value );
	bool setUniform( string uniformName, vec4 value );
	bool setUniform( string uniformName, float value );

	bool setVBO( string attributeName,  
				 vector<GLfloat> data, 
				 GLenum usage = GL_STATIC_DRAW );
	
	bool setVBO( string attributeName,  
				 vector<GLint> data, 
				 GLenum usage = GL_STATIC_DRAW );

	bool setVBO( string attributeName,  
				 GLfloat data[], 
				 int dataLength, 
				 GLenum usage = GL_STATIC_DRAW );

	bool setVBO( string attributeName,
				 GLint data[],
				 int dataLength,
				 GLenum usage = GL_STATIC_DRAW );
	
	bool shareExistingVBO( string attributeName, 
						   GLuint attributeindex,
						   GLuint vboHandle );

	void use();

private:	
	bool setVBO( string attributeName,  
				 GLvoid* data, 
				 int sizeOfData,
				 GLenum usage );

};

#endif