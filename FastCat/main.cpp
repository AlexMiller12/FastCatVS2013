#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\glut.h"
#include "GLUTWrapper.h"

//standard glut-based program functions
void init( void );
void resize( int, int );
void display( void );
void keypress( unsigned char, int, int );
void cleanup( void );

void setDisplayFunction( void( *f )() );
void blarg();

int main( int numArguments, char** arguments )
{
	char* path = "C:/Users/Alexander/Desktop/CIS660/FastCatAuthoringTool/x64/Debug/FastCat.exe";
	GLUTWrapper glut;
	glut.basicGLUTWindow( arguments[0] );

	glut.setDisplayFunction( display );
	glut.startLoop();
	return 0;
}

void display() 
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
	glRotatef( 20, 0, 1, 0 );
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
