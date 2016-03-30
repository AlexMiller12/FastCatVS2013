#ifndef IO_UTIL
#define IO_UTIL

#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h> // To get execution path

using namespace std;

class IOUtil
{
	
//----------------------------------------------------------------------------ENUMS:
	
//------------------------------------------------------------------------CONSTANTS:
	
private:
	static const int MAX_FILEPATH_LENGTH = 300;

//---------------------------------------------------------------------------FIELDS:
	
//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
		
//------------------------------------------------------------------------FUNCTIONS:

//--------------------------------------------------------------------------METHODS:

public:
	static string executionPath();
	static bool readWholeFile( const char* fileName, string& buffer );
	static bool readWholeFile( string fileName, string& buffer );

};

#endif