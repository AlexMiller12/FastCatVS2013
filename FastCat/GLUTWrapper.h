#ifndef GLUT_WRAPPER_H_
#define GLUT_WRAPPER_H_

#include <stdlib.h>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\glut.h"

class GLUTWrapper
{
//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:

public:
private:
	
//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	GLUTWrapper();
	virtual ~GLUTWrapper();
	
//--------------------------------------------------------------------------METHODS:


public:
	void setDisplayFunction( void( *f )( ) );
	void basicGLUTWindow( char* arguments );
	void startLoop();
protected:
private:

	// Standard glut-based program functions to be passed to glut when initialized
	//void cleanup( void );
	//void defaultDisplay( void );
	//void init( void );
	//void keypress( unsigned char, int, int );
	//void resize( int, int );
};
#endif