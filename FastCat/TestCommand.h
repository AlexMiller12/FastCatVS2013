/*
filename:
hello_maya.h
description:
This file contains the definition for a new command which the user of this plugin
will be able to call from within Maya. The definitions for the class methods are
contained in hello_maya.cpp.
*/

#ifndef TEST_COMMAND_H_
#define TEST_COMMAND_H_

#include <maya\MPxCommand.h>
#include <maya\MGlobal.h>
//#include <MFnPlugin.h>
//#include <MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include "FastCatRenderer.h"

// A very, very basic Maya command.
class TestCommand : public MPxCommand 
{	
//------------------------------------------------------------------------CONSTANTS:

//---------------------------------------------------------------------------FIELDS:

public:

private:
	FastCatRenderer renderer;


//---------------------------------------------------------------------CONSTRUCTORS:

public:
	TestCommand() {};
	
//--------------------------------------------------------------------------METHODS:

public:
	// This method is called when the command is registered.
	// It should return a pointer to a new instance of the class.
	static void* creator();

	// This is the function that gets called when the Maya user calls 'HelloMaya' from MEL.
	virtual MStatus	doIt( const MArgList& );

	// Necessary for passing arguments
	static MSyntax newSyntax();
};


#endif