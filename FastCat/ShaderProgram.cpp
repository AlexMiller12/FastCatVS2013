#include "ShaderProgram.h"

//---------------------------------------------------------------------------FIELDS:

unordered_map<string, GLint> ShaderProgram::sboHandles;

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::~ShaderProgram()
{
	// TODO, clean up!
}

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

// Compiles and attaches shader with given source and type
bool ShaderProgram::attachShader( const char* source, GLenum type )
{
	bindToVAO();
	GLuint shaderHandle = glCreateShader( type );
	glShaderSource( shaderHandle, 1, &source, NULL );
	glCompileShader( shaderHandle );

	// Save the handle to the shader so we can delete after linking 
	// (Or for printing errors)
	shaders.push_back( shaderHandle );
	//TODO: only on debug mode
	int succeeded;
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &succeeded );
	if( !succeeded )
	{
		printShaderErrors();
		GLUtil::printErrors();
		return false;
	}

	glAttachShader( handle, shaderHandle );

	if( DEBUG )   return !GLUtil::printErrors() && !printShaderErrors();
	return true;
}

// Compiles and attaches shader with given source and type
bool ShaderProgram::attachShader( string source, GLenum type )
{
	return attachShader( source.c_str(), type );
}

//bool ShaderProgram::bindToIndexBuffer()
//{
//	
//}

// Binds to the vertex array object associated with this program
bool ShaderProgram::bindToVAO()
{
	glBindVertexArray( vertexArrayObjectHandle );

	if( DEBUG )  return ! GLUtil::printErrors();

	return true;
}

bool ShaderProgram::createSBO( string name )
{
	GLuint ssboHandle = 0;
	glGenBuffers( 1, &ssboHandle );
	sboHandles[name] = ssboHandle;

	if( DEBUG )   return GLUtil::printErrors();
	return true;
}

// Initializes a vertex buffer and assigns given attribute index (for when we want
// to create a pointer to buffer)
bool ShaderProgram::createVBO( string attributeName, GLuint attributeindex )
{
	// Bind to VAO so we assign new VBO to it
	bindToVAO();

	// Generate new buffer
	GLuint newVBOHandle;
	glGenBuffers( 1, &newVBOHandle );

	if( DEBUG  &&  GLUtil::printErrors() )
	{
		return false;
	}

	// Store handle for quick lookup later
	attributeLocations[attributeName] = newVBOHandle;

	attributeIndices[attributeName] = attributeindex;

	// Assign it to "in vec3" variable in shader
	glBindAttribLocation( handle, attributeindex, attributeName.c_str() );

	return ! GLUtil::printErrors();
}

// Enables an attribute pointer to buffer with given name
bool ShaderProgram::enableFloatAttribute( string attributeName, int floatsPerVertex )
{
	GLuint bufferHandle = getAttributeLocation( attributeName );

	GLuint attributeIndex = attributeIndices[attributeName]; 
	
	// Enable attribute
	glEnableVertexAttribArray( attributeIndex );

	glBindBuffer( GL_ARRAY_BUFFER, bufferHandle );

	//GLsizei stride = floatsPerVertex * sizeof( float );

	// Tell GL how to handle data in buffer
	glVertexAttribPointer( attributeIndex,
							floatsPerVertex,
							GL_FLOAT,
							GL_FALSE, // normalized?
							0,		 // stride 
							(void*)0 );      // array buffer offset

	if( DEBUG )  return ! GLUtil::printErrors();
	return true;
}

// Enables an attribute pointer to buffer with given name
bool ShaderProgram::enableIntAttribute( string attributeName, int intsPerVertex )
{
	GLuint bufferHandle = getAttributeLocation( attributeName );

	GLuint attributeIndex = attributeIndices[attributeName];

	// Enable attribute
	glEnableVertexAttribArray( attributeIndex );

	glBindBuffer( GL_ARRAY_BUFFER, bufferHandle );

	//GLsizei stride = intsPerVertex * sizeof( int );

	// Tell GL how to handle data in buffer
	glVertexAttribPointer( attributeIndex,
						   intsPerVertex,
						   GL_INT,
						   GL_FALSE, // normalized?
						   0,		 // stride 
						   (void*)0 );      // array buffer offset

	if( DEBUG )  return ! GLUtil::printErrors();
	return true;
}

bool ShaderProgram::enableInt1Attribute( string attributeName )
{
	// enable Attribute with 3 floats per vertex
	return enableIntAttribute( attributeName, 1 );
}

// Enables an attribute pointer to buffer with given name
bool ShaderProgram::enableVec1Attribute( string attributeName )
{
	// enable Attribute with 3 floats per vertex
	return enableFloatAttribute( attributeName, 1 );
}

// Enables an attribute pointer to buffer with given name
bool ShaderProgram::enableVec3Attribute( string attributeName )
{
	// enable Attribute with 3 floats per vertex
	return enableFloatAttribute( attributeName, 3 );
}

// Enables an attribute pointer to buffer with given name
bool ShaderProgram::enableVec4Attribute( string attributeName )
{
	// enable Attribute with 4 floats per vertex
	return enableFloatAttribute( attributeName, 4 );
}

// Prepares shader for use (links program and deletes shaders)
bool ShaderProgram::finalizeProgram()
{
	glLinkProgram( handle );

	// TODO go through shaders and detach and delete them
	//		glDetachShader( geomFragProgram, fragShader );
	//		glDeleteShader( geomShader );


	if( DEBUG )   return !GLUtil::printErrors() && !printProgramErrors();
	return true;
}

// Returns the handle of the attribute associated with given name
GLuint ShaderProgram::getAttributeLocation( string name )
{
	// Look up the stored uniform location
	GLint attributeLocation = attributeLocations[name];

	// If it has not yet been stored, we must query GL for location.  We don't 
	// simply query every time because it is much more expensive than a simple 
	// hash map lookup
	if( ! attributeLocation )
	{
		// Query location
		attributeLocation = glGetAttribLocation( handle, name.c_str() );

		if( attributeLocation == -1 )   GLUtil::printErrors();

		// Store for later use
		attributeLocations[name] = attributeLocation;
	}
	return attributeLocation;
}

GLuint ShaderProgram::getBlockIndex( string name )
{
	// Look up the stored SSBO block index 
	GLint blockIndex = sboBlockIndices[name];

	// If it has not yet been stored, we must query GL for location.  We don't 
	// simply query every time because it is much more expensive than a simple 
	// hash map lookup
	if( ! blockIndex )
	{
		blockIndex = glGetProgramResourceIndex( handle,
												GL_SHADER_STORAGE_BLOCK,
												name.c_str() );
		sboBlockIndices[name] = blockIndex;
	}
	return blockIndex;
}

GLuint ShaderProgram::getSBOHandle( string name )
{
	// Look up the stored uniform location
	return sboHandles[name];
}

// Returns the handle of this program
GLuint ShaderProgram::getHandle()
{
	return handle;
}

// Returns the handle of the unifrom or attribute associated with given name
GLuint ShaderProgram::getUniformLocation( string name )
{
	// Look up the stored uniform location
	GLint uniformLocation = uniformLocations[name];

	// If it has not yet been stored, we must query GL for location.  We don't 
	// simply query every time because it is much more expensive than a simple 
	// hash map lookup
	if( !uniformLocation )
	{
		// Query location
		uniformLocation = glGetUniformLocation( handle, name.c_str() );

		if( uniformLocation == -1 )
		{
			printf( "Error setting uniform location for: %s.\nName does not "
				"correspond to an active uniform variable in program, the "
				"name starts with the reserved prefix 'gl_', or it is "
				"associated with an atomic counter or a named uniform block.\n",
				name.c_str() );
			return uniformLocation;
		}
		// Store for later use
		uniformLocations[name] = uniformLocation;
	}
	return uniformLocation;
}

// Initializes index buffer
bool ShaderProgram::init( bool createIndexBuffer )
{
	handle = glCreateProgram();
	if( DEBUG )  GLUtil::printErrors();
	glGenVertexArrays( 1, &vertexArrayObjectHandle );
	// TODO: return false on error
	if( createIndexBuffer )
	{
		glGenBuffers( 1, &indexBufferHandle );
	}
	return ! GLUtil::printErrors();
}

// Initializes index buffer
bool ShaderProgram::init( GLuint vaoHandle, bool createIndexBuffer )
{
	handle = glCreateProgram();
	if( DEBUG )  GLUtil::printErrors();
	vertexArrayObjectHandle = vaoHandle;
	// TODO: return false on error
	if( createIndexBuffer )
	{
		glGenBuffers( 1, &indexBufferHandle );
	}
	return ! GLUtil::printErrors();
}

// Prints any program errors.  Returns true if there were errors
bool ShaderProgram::printProgramErrors()
{
	int linkStatus;
	glGetProgramiv( handle, GL_LINK_STATUS, (int *)&linkStatus );
	if( !linkStatus )
	{
		GLint expectedLength = 0;
		glGetProgramiv( handle, GL_INFO_LOG_LENGTH, &expectedLength );
		vector<GLchar> errorLog( expectedLength );
		GLsizei actualLength;
		glGetProgramInfoLog( handle, expectedLength, &actualLength, &errorLog[0] );
		printf( "Log:\n%s", &errorLog[0] );
		return true;
	}
	// no errors
	return false;
}

// Prints any shader errors.  Returns true if there were errors
bool ShaderProgram::printShaderErrors()
{
	for( int i = 0; i < shaders.size(); i++ )
	{
		GLuint shaderHandle = shaders.at( i );
		int status;
		glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &status );
		if( !status )
		{
			GLint expectedLength = 0;
			glGetShaderiv( shaderHandle, GL_INFO_LOG_LENGTH, &expectedLength );
			vector<GLchar> errorLog( expectedLength );
			GLsizei actualLength;
			glGetShaderInfoLog( shaderHandle, expectedLength, &actualLength, &errorLog[0] );
			printf( "Log:\n%s", &errorLog[0] );
			return true;
		}
	}
	// no errors
	return false;
}

bool ShaderProgram::setIndices( vector<GLuint> indices, GLenum usage )
{
	numIndices = indices.size();
	return setIndices( &indices[0], indices.size(), usage );
}

bool ShaderProgram::setIndices( vector<GLushort> indices, GLenum usage )
{
	numIndices = indices.size();
	return setIndices( &indices[0], indices.size(), usage );
}

bool ShaderProgram::setIndices( GLuint indices[], int indicesLen, GLenum usage )
{
	numIndices = indicesLen;
	GLushort bufferSize = numIndices * sizeof( GLuint );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferHandle );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, bufferSize, indices, usage );

	if( DEBUG )  return ! GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setIndices( GLushort indices[], int indicesLen, GLenum usage )
{
	numIndices = indicesLen;
	GLushort bufferSize = numIndices * sizeof( GLushort );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferHandle );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, bufferSize, indices, usage );

	if( DEBUG )  return ! GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setSBO( string name,
							GLsizei size,
							const void* data,
							GLenum usage )
{
	GLuint ssboHandle = getSBOHandle( name );

	glBindBuffer( GL_SHADER_STORAGE_BUFFER, ssboHandle );
	glBufferData( GL_SHADER_STORAGE_BUFFER, size, data, usage );

	GLvoid* p = glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY );
	memcpy( p, data, size );
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	if( DEBUG )  return ! GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setSBOBindingPoint( GLuint bindingPointIndex, string sboName )
{
	GLuint sboHandle = getSBOHandle( sboName );
	return setSBOBindingPoint( bindingPointIndex, sboName, sboHandle );
}

bool ShaderProgram::setSBOBindingPoint( GLuint bindingPointIndex, string sboName, GLuint sboHandle )
{
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, sboHandle );

	GLuint blockIndex = getBlockIndex( sboName );
	glShaderStorageBlockBinding( handle, blockIndex, bindingPointIndex );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER, bindingPointIndex, sboHandle );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setUniform( string uniformName, float value )
{
	use();
	GLuint uniformLocation = getUniformLocation( uniformName );
	glUniform1f( uniformLocation, value );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setUniform( string uniformName, mat3 value )
{
	use();
	GLuint uniformLocation = getUniformLocation( uniformName );
	glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, &value[0][0] );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setUniform( string uniformName, mat4 value )
{
	use();
	GLuint uniformLocation = getUniformLocation( uniformName );
	glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &value[0][0] );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setUniform( string uniformName, vec3 value )
{
	use();
	GLuint uniformLocation = getUniformLocation( uniformName );
	glUniform3f( uniformLocation, value.x, value.y, value.z );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setUniform( string uniformName, vec4 value )
{
	use();
	GLuint uniformLocation = getUniformLocation( uniformName );
	glUniform4f( uniformLocation, value.x, value.y, value.z, value.w );

	if( DEBUG )  return !GLUtil::printErrors();
	return true;
}

bool ShaderProgram::setVBO( string attributeName,
							vector<GLfloat> data,
							GLenum usage )
{
	return setVBO( attributeName, &data[0], data.size(), usage );
}

bool ShaderProgram::setVBO( string attributeName,
							vector<GLint> data,
							GLenum usage )
{
	return setVBO( attributeName, &data[0], data.size(), usage );
}

bool ShaderProgram::setVBO( string attributeName,
							GLfloat data[],
							int dataLength,
							GLenum usage )
{
	return setVBO( attributeName, 
				   (GLvoid*)data, 
				   dataLength * sizeof( GLfloat ),
				   usage );
}

bool ShaderProgram::setVBO( string attributeName,
							GLint data[],
							int dataLength,
							GLenum usage )
{
	return setVBO( attributeName, 
				   (GLvoid*)data, 
				   dataLength * sizeof( GLint ),
				   usage );
}

bool ShaderProgram::shareExistingVBO( string attributeName, 
									  GLuint attributeIndex,
									  GLuint vboHandle )
{
	// Store handle for quick lookup later
	attributeLocations[attributeName] = vboHandle;

	attributeIndices[attributeName] = attributeIndex;

	// Assign it to "in vec3" variable in shader
	glBindAttribLocation( handle, attributeIndex, attributeName.c_str() );

	return ! GLUtil::printErrors();
}

void ShaderProgram::use()
{
	glUseProgram( handle );
}

//--------------------------------------------------------------------------HELPERS:

bool ShaderProgram::setVBO( string attributeName,
							GLvoid* data,
							int sizeOfData,
							GLenum usage )
{
	// If the VBO has not been created, return false
	if( ! attributeLocations[attributeName] )   return false;

	// Bind to VAO so we assign new VBO to it
	bindToVAO();

	// Find handle to data buffer
	GLuint bufferHandle = getAttributeLocation( attributeName );

	// Bind to the VBO
	glBindBuffer( GL_ARRAY_BUFFER, bufferHandle );

	// Set the data of the buffer
	glBufferData( GL_ARRAY_BUFFER,
				  sizeOfData,
				  data,
				  usage );

	if( DEBUG )
	{
		return ! GLUtil::printErrors();
	}
	return true;
}





