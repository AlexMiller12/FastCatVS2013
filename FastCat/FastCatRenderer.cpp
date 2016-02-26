
#include "FastCatRenderer.h"

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:


FastCatRenderer::FastCatRenderer()
{

}

FastCatRenderer::~FastCatRenderer()
{

}

//--------------------------------------------------------------------------METHODS:

void FastCatRenderer::init()
{
	glut.basicGLUTWindow();
}

//TODO: temp
void FastCatRenderer::test()
{
	testPass();
}

//--------------------------------------------------------------------------HELPERS:

//TODO TEMP:
int rot = 20; 
// Function to render hello world (to be passed to glut)
void FastCatRenderer::testPass()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable( GL_DEPTH_TEST );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -2.0, 2.0, -2.0, 2.0, -2.0, 500.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( 2, 2, 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	glScalef( .005, .005, .005 );
	glRotatef( rot++, 0, 1, 0 );
	glRotatef( 30, 0, 0, 1 );
	glRotatef( 5, 1, 0, 0 );
	glTranslatef( -300, 0, 0 );

	glColor3f( 1, 1, 1 );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'H' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'e' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'o' );

	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'W' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'o' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'r' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'l' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, 'd' );
	glutStrokeCharacter( GLUT_STROKE_ROMAN, '!' );

	glutSwapBuffers();
}
