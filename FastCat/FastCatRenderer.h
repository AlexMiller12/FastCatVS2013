#include "GLUTWrapper.h"
class FastCatRenderer
{
//------------------------------------------------------------------------CONSTANTS:
	
//---------------------------------------------------------------------------FIELDS:

public:
private:
	GLUTWrapper glut;
	
//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:
	
public:
	FastCatRenderer();
	virtual ~FastCatRenderer();
	
//--------------------------------------------------------------------------METHODS:

public:
	void init();
	void test();

private:
	void testPass();
};