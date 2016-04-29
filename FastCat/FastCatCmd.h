#ifndef FAST_CAT_CMD_H
#define FAST_CAT_CMD_H

#include <maya/MPxCommand.h>
#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

// TEMP:
#include <maya/MPlugArray.h>
#include <maya/MPlug.h> 
#include <maya/MTextureManager.h> 
#include <maya/MViewport2Renderer.h>

using namespace MHWRender;


#define FAST_CAT_CMD_NAME "fastcat"


class FastCatCmd : public MPxCommand
{
public:
	FastCatCmd() {};
	virtual ~FastCatCmd() {};

	static void *creator() { return new FastCatCmd; }
	static MSyntax newSyntax();

	virtual MStatus doIt(const MArgList &args);
};

#endif // FAST_CAT_CMD_H