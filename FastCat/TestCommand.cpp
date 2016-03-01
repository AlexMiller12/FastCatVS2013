/*
filename:
hello_maya.cpp
description :
Contains definitions for the HelloMaya class methods declared in hello_maya.h.
*/

#include "TestCommand.h"

#include <maya\MGlobal.h>
#include <string>
#include "Singleton.h"

//------------------------------------------------------------------------CONSTANTS:
// For setting up visual studio:
//     https://beesoverdundee.wordpress.com/2015/04/02/tutorial-making-maya-2015-plugins-with-visual-studio/
// For passing arguments:
//     https://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/maya/MSyntax.html

const char* idFlag = "-id";
const char* idFlagLong = "-identification number";

const char* nameFlag = "-n";
const char* nameFlagLong = "-username";

//--------------------------------------------------------------------------METHODS:

// This function is called when the plugin is registered with the Maya API.
// All it does is return a pointer to a new instance of the class.
void* TestCommand::creator()
{
	return new TestCommand;
}

// This function is called when the Maya user types 'TestCommand' in MEL.
MStatus TestCommand::doIt( const MArgList& args )
{
	// Create default values for arguments we will try to parse 
	MStatus status = MS::kSuccess;
	FastCatRenderer renderer = Singleton<FastCatRenderer>::instance();

	renderer.createWindow();
	renderer.init();
	renderer.test();

	//renderer.closeWindow();
	return MS::kSuccess;
}

MSyntax TestCommand::newSyntax()
{
	MSyntax syntax;

	syntax.addFlag( idFlag, idFlagLong, MSyntax::kDouble );
	syntax.addFlag( nameFlag, nameFlagLong, MSyntax::kString );

	return syntax;
}