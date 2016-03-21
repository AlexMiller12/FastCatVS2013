#include "GLUtil.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

bool GLUtil::isError()
{
	GLenum errorStatus = glGetError();
	bool errors = errorStatus != GL_NO_ERROR;
	if( errors )
	{
		printf( "oh no!" ); //TODO temp
	}
	return glGetError() != GL_NO_ERROR;
}

int GLUtil::maxNumPatches()
{
	GLint MaxPatchVertices = 0;
	glGetIntegerv( GL_MAX_PATCH_VERTICES, &MaxPatchVertices );
	return MaxPatchVertices;
}
// Returns true if error existed and was printed, false if no error.
bool GLUtil::printErrors()
{
	GLenum errorStatus = glGetError();
	if( errorStatus == GL_NO_ERROR )   return false;
	
	GLenum error( glGetError() );

	string errorMessage;
	switch( errorStatus )
	{
	case 0x0502:
		errorMessage = "INVALID_OPERATION";      
		break;
	case 0x0500:
		errorMessage = "INVALID_ENUM";           
		break;
	case 0x0501:
		errorMessage = "INVALID_VALUE";          
		break;
	case 0x0505:
		errorMessage = "OUT_OF_MEMORY";         
		break;
	case 0x0506:
		errorMessage = "INVALID_FRAMEBUFFER_OPERATION"; 
		break;

	case 0x0503:
		errorMessage = "GL_STACK_OVERFLOW";
		break;
	case 0x0504:
		errorMessage = "GL_STACK_UNDERFLOW";
		break;
	case GL_CONTEXT_LOST:
		errorMessage = "GL_CONTEXT_LOST";
		break;
	default:
		errorMessage = "Unrecognized error %d", errorStatus;
		break;
	}
	printf( errorMessage.c_str() );
	return true;
}

//--------------------------------------------------------------------------HELPERS:
