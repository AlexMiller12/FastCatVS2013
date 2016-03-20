#include "FastCatCmd.h"
#include "FastCatRenderer.h"
#include "Camera.h"

#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MItMeshPolygon.h>
#include <memory>


MSyntax FastCatCmd::newSyntax()
{
	MSyntax syntax;

	// Define command arguments here by syntax.addFlag(shortName, longName, MSyntax::someType)

	return syntax;
}


// Get mesh data from the selected mesh
// Fail if the selected is not mesh or there are more other than one selection
MStatus FastCatCmd::doIt(const MArgList &args)
{
	// For testing only
	static FastCatRenderer testRenderer;

	MSelectionList curSel;
	MGlobal::getActiveSelectionList(curSel);

	if (curSel.length() == 0)
	{
		MGlobal::displayError("A polymesh must be selected");
		return MS::kFailure;
	}
	if (curSel.length() > 1)
	{
		MGlobal::displayError("Only one polymesh is supported right now");
		return MS::kFailure;
	}

	MDagPath dagPath;
	MObject node; // a node in the DAG path

	curSel.getDagPath(0, dagPath);
	if (dagPath.extendToShape() != MS::kSuccess)
	{
		MGlobal::displayError("Selection is not a polymesh");
		return MS::kFailure;
	}

	node = dagPath.node();

	std::shared_ptr<ControlMesh> mesh = std::make_shared<ControlMesh>();
	mesh->initBaseMeshFromMaya(node);
	mesh->maxSubdivisionLevel = 3;

	testRenderer.testMesh = mesh;
	if (!testRenderer.isReady)
	{
		testRenderer.createWindow();
		testRenderer.init(); // load shaders and create a program
		testRenderer.camera = Camera::createCamera(WINDOW_WIDTH, WINDOW_HEIGHT);
		testRenderer.isReady = true;
	}
	testRenderer.test();

	return MS::kSuccess;
}