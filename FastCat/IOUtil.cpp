#include "IOUtil.h"


//--------------------------------------------------------------------------METHODS:

string IOUtil::executionPath()
{
	char buffer[MAX_FILEPATH_LENGTH];
	GetModuleFileName( NULL, buffer, MAX_FILEPATH_LENGTH );
	string::size_type pos = string( buffer ).find_last_of( "\\/" );
	return string( buffer ).substr( 0, pos );
}

bool IOUtil::readWholeFile( const char* fileName, string& buffer )
{
	std::ifstream ifs( fileName, std::ifstream::in | std::ifstream::binary );

	if( !ifs )
	{
		return false;
	}

	ifs.seekg( 0, std::ifstream::end );
	int length = ifs.tellg();
	ifs.seekg( 0, std::ifstream::beg );

	buffer.resize( length );
	ifs.read( &buffer[0], length );

	ifs.close();
	return true;
}

bool IOUtil::readWholeFile( string fileName, string& buffer )
{
	return readWholeFile( fileName.c_str(), buffer );
}

//--------------------------------------------------------------------------HELPERS:
