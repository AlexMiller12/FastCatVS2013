#ifndef BASIC_SHADERS
#define BASIC_SHADERS

class BasicShaders
{
	// Most copied and pasted from:
	// http://ogldev.atspace.co.uk/www/tutorial30/tutorial30.html

//------------------------------------------------------------------------CONSTANTS:

public:
	static const char* noMVP_v;
	// Tessellation Shaders:
	static const char* flat_te;
	static const char* simple_te;
	static const char* simple_tc;
	// Geometry Shaders:
	static const char* simple_g;
	// Fragment Shaders:
	static const char* lambert_f;
};
#endif