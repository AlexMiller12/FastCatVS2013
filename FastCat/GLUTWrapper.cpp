#include "GLUTWrapper.h"

//---------------------------------------------------------------------------FIELDS:

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

GLUTWrapper::GLUTWrapper() {}

GLUTWrapper::~GLUTWrapper() {}

//-----------------------------------------------------------DEFAULT WINDOW DISPLAY:

void defaultDisplay( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glutSwapBuffers();
}

//--------------------------------------------------------------------------METHODS:


void GLUTWrapper::basicGLUTWindow()
{
	// glutInit thinks it's getting command line arguments
	int numArgs = 1;
	char* arguments[] = { "why do they even need this?" };
	glutInit( &numArgs, arguments );

	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( 500, 500 );
	glutInitWindowPosition( 300, 200 );
	glutCreateWindow( "Hello World!" );
	glutDisplayFunc( defaultDisplay );
}

void GLUTWrapper::setDisplayFunction( void( *f )( ) )
{
	glutDisplayFunc( ( *f ) );
}

void GLUTWrapper::startLoop()
{
	glutMainLoop();
}


//--------------------------------------------------------------------------HELPERS:
