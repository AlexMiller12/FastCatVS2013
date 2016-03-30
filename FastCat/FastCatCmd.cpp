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

#include <Windows.h>
#include <process.h>
#include <sstream>


MSyntax FastCatCmd::newSyntax()
{
	MSyntax syntax;

	// Define command arguments here by syntax.addFlag(shortName, longName, MSyntax::someType)

	return syntax;
}


struct ThreadDataGL
{
	float baseTessFactor;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<ControlMesh> mesh;
} threadDataGL;

HANDLE handleThreadGL = NULL;
HANDLE handleCloseWindowEvent = NULL;


unsigned int __stdcall threadProcGL(void* data)
{
	ThreadDataGL *pData = reinterpret_cast<ThreadDataGL *>(data);

	FastCatRenderer mainRenderer(pData->baseTessFactor, pData->mesh, pData->camera);

	if (!mainRenderer.isReady)
	{
		mainRenderer.createWindow();
		mainRenderer.init(); // load shaders and create a program
		mainRenderer.isReady = true;
	}

	while (!glfwWindowShouldClose(mainRenderer.window) &&
		WaitForSingleObject(handleCloseWindowEvent, 0) != WAIT_OBJECT_0)
	{
		// render one frame
		mainRenderer.test();

		glfwPollEvents();
		glfwSwapBuffers(mainRenderer.window);
	}

	mainRenderer.closeWindow();

	return 0;
}

// Get mesh data from the selected mesh
// Fail if the selected is not mesh or there are more other than one selection
MStatus FastCatCmd::doIt(const MArgList &args)
{
	// For testing only
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

	std::shared_ptr<Camera> camera = Camera::createCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	float baseTessFactor = 16.0f;
	std::shared_ptr<ControlMesh> mesh = std::make_shared<ControlMesh>();
	mesh->initBaseMeshFromMaya(node);
	mesh->maxSubdivisionLevel = ceil(log2(baseTessFactor));

	if (!handleCloseWindowEvent)
	{
		handleCloseWindowEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("CloseWindowEvent"));
		if (!handleCloseWindowEvent)
		{
			MGlobal::displayError("Cannot create CloseWindowEvent");
			return MS::kFailure;
		}
	}

	if (handleThreadGL)
	{
		// signal the thread to terminate
		if (!SetEvent(handleCloseWindowEvent))
		{
			MGlobal::displayError("Failed to set event CloseWidnowEvent");
			return MS::kFailure;
		}

		// wait until it has terminated
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(handleThreadGL, INFINITE);

		if (dwWaitResult != WAIT_OBJECT_0)
		{
			std::stringstream ss;
			ss << "WaitForSingleObject failed (" << GetLastError() << ")";
			MGlobal::displayError(ss.str().c_str());
			return MS::kFailure;
		}

		if (!ResetEvent(handleCloseWindowEvent))
		{
			MGlobal::displayError("Failed to reset event CloseWidnowEvent");
			return MS::kFailure;
		}

		// resource is not release before the handle is closed
		CloseHandle(handleThreadGL);
		handleThreadGL = NULL;
	}

	// create a separate thread for OpenGL rendering
	threadDataGL.baseTessFactor = baseTessFactor;
	threadDataGL.camera = camera;
	threadDataGL.mesh = mesh;

	handleThreadGL = (HANDLE)_beginthreadex(0, 0, threadProcGL, &threadDataGL, 0, 0);

	return MS::kSuccess;
}