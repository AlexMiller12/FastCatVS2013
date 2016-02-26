/*
filename:
plugin_main.cpp
description:
Contains two functions, initializePlugin and uninitializePlugin, which are called
by Maya when it loads and unloads the plugin. As such, this file can be thought of
as a 'front-end' for the plugin, similar to the way a 'main' file is the entry point
to a program.

Each time a new custom command is created and added to the plugin it needs to be
registered and deregistered by these functions. The same goes for custom nodes.
*/

#include <maya\MFnPlugin.h>

#include "TestCommand.h"

//------------------------------------------------------------------------FUNCTIONS:

// Registers all the plugin's commands, nodes etc with the Maya API.
MStatus initializePlugin( MObject obj )
{
	MFnPlugin plugin( obj, "Alexander Miller", "2016" );

	MStatus	status;

	// Register HelloMaya command with the Maya API.
	status = plugin.registerCommand( "TestCommand",
									 TestCommand::creator,
									 TestCommand::newSyntax );

	if( ! status )
	{
		status.perror( "registerCommand failed" );
	}

	return status;
}

// De-registers all the plugin's commands, nodes etc from the Maya API.
MStatus uninitializePlugin( MObject obj )
{
	MFnPlugin plugin( obj );
	MStatus	status;

	// Remove HelloMaya command from the Maya API environment.
	status = plugin.deregisterCommand( "TestCommand" );

	if( ! status )
	{
		status.perror( "deregisterCommand failed" );
	}
	return status;
}