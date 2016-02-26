#include "FastCatRenderer.h"

//-----------------------------------------------------------------------------MAIN:

int main( int numArguments, char** arguments )
{
	FastCatRenderer renderer;

	renderer.init();
		
	while( true )
	{
		renderer.test();
	}
	return 0;
}

//------------------------------------------------------------------------FUNCTIONS:
