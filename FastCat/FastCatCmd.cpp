#include "FastCatCmd.h"
#include "FastCatRenderer.h"
#include "Camera.h"

#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MThreadAsync.h>
#include <maya/MSpinLock.h>
#include <maya/MArgDatabase.h>

#include <Windows.h>
#include <process.h>
#include <sstream>
#include <memory>


const char *subdLevelFlag_s = "-lvl", *subdLevelFlag_l = "-level";


MSyntax FastCatCmd::newSyntax()
{
	MSyntax syntax;

	// Define command arguments here by syntax.addFlag(shortName, longName, MSyntax::someType)
	syntax.addFlag(subdLevelFlag_s, subdLevelFlag_l, MSyntax::kLong);

	return syntax;
}

//-----TEMP
MPlug getDisplacementMapPlug( const MDagPath& dagPath )
{
	MStatus status = MStatus::kFailure;

	if( dagPath.hasFn( MFn::kMesh ) )
	{
		// Find the Shading Engines Connected to the SourceNode 
		MFnMesh fnMesh( dagPath.node() );
		// A ShadingGroup will have a MFnSet 
		MObjectArray sets, comps;
		fnMesh.getConnectedSetsAndMembers( dagPath.instanceNumber(),
			sets,
			comps,
			true );
		// Each set is a Shading Group. Loop through them
		for( unsigned int i = 0; i < sets.length(); ++i )
		{
			MFnDependencyNode fnDepSGNode( sets[i] );

			MPlug displacementPlug = fnDepSGNode.findPlug( "displacementShader",
				true,
				&status );
			if( status == MStatus::kSuccess )
			{
				return displacementPlug;
			}
		}
	}

	return MPlug();
}

MTexture* getDisplacementMapTexture( const MDagPath& dagPath, int width, int height )
{
	MPlug displacementPlug = getDisplacementMapPlug( dagPath );

	MRenderer* theRenderer = MRenderer::theRenderer( false );
	MTextureManager* textureManager = theRenderer->getTextureManager();

	return textureManager->acquireTexture( "blinn1", displacementPlug, width, height );
}
//-----


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

	while (!glfwWindowShouldClose(mainRenderer.window))
	{
		if (WaitForSingleObject(handleCloseWindowEvent, 0) == WAIT_OBJECT_0)
		{
			mainRenderer.changeControlMesh(threadDataGL.baseTessFactor, threadDataGL.mesh);

			if (!ResetEvent(handleCloseWindowEvent))
			{
				MGlobal::displayError("Failed to reset event CloseWidnowEvent");
				return MS::kFailure;
			}
		}

		// render one frame
		mainRenderer.test();

		glfwPollEvents();
		glfwSwapBuffers(mainRenderer.window);
	}

#ifdef FAST_CAT_DEBUG_MODE
	mainRenderer.controlMesh->clearDebugBuffers();
#endif
	mainRenderer.closeWindow();

	return 0;
}

// Get mesh data from the selected mesh
// Fail if the selected is not mesh or there are more other than one selection
MStatus FastCatCmd::doIt(const MArgList &args)
{
	// Get options
	MArgDatabase argDB(syntax(), args);
	int subdLevel = 1;

	if (argDB.isFlagSet(subdLevelFlag_s))
	{
		argDB.getFlagArgument(subdLevelFlag_s, 0, subdLevel);

		if (subdLevel < 0)
		{
			MGlobal::displayError("Invalid subdivision level");
			return MS::kFailure;
		}
		if (subdLevel > 6)
		{
			MGlobal::displayWarning("Only support upto 6 level of subdivisions");
			subdLevel = 6;
		}
	}

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

	curSel.getDagPath(0, dagPath);
	if (dagPath.extendToShape() != MS::kSuccess)
	{
		MGlobal::displayError("Selection is not a polymesh");
		return MS::kFailure;
	}

	//----------------TEMP 

	//MTexture* displacementMapTexture = getDisplacementMapTexture( dagPath, 512, 512 );
	//int rowPitch, slicePitch;
	//unsigned char* rawData = (unsigned char*)displacementMapTexture->rawData( rowPitch, slicePitch );
	//int bytesPerPixel = displacementMapTexture->bytesPerPixel();
	//int width = rowPitch / bytesPerPixel;
	//int height = ( slicePitch / bytesPerPixel ) / width;
	//MTextureDescription textureDescription;

	//displacementMapTexture->textureDescription( textureDescription );

	//for( int i = 0; i < 512; i += 4 )
	//{
	//	unsigned char r = rawData[i];
	//	unsigned char g = rawData[i + 1];
	//	unsigned char b = rawData[i + 2];
	//	unsigned char a = rawData[i + 3];
	//	int blarg = 0;
	//}

	//----------------


	std::shared_ptr<Camera> camera = Camera::createCamera(WINDOW_WIDTH, WINDOW_HEIGHT);

	float baseTessFactor = static_cast<float>(1 << subdLevel);
	std::shared_ptr<ControlMesh> mesh = std::make_shared<ControlMesh>();
	mesh->initBaseMeshFromMaya(dagPath);
	mesh->maxSubdivisionLevel = ceil(log2(baseTessFactor));

	// create a separate thread for OpenGL rendering
	threadDataGL.baseTessFactor = baseTessFactor;
	threadDataGL.camera = camera;
	threadDataGL.mesh = mesh;

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
		//DWORD dwWaitResult;
		//dwWaitResult = WaitForSingleObject(handleThreadGL, INFINITE);

		//if (dwWaitResult != WAIT_OBJECT_0)
		//{
		//	std::stringstream ss;
		//	ss << "WaitForSingleObject failed (" << GetLastError() << ")";
		//	MGlobal::displayError(ss.str().c_str());
		//	return MS::kFailure;
		//}

		//if (!ResetEvent(handleCloseWindowEvent))
		//{
		//	MGlobal::displayError("Failed to reset event CloseWidnowEvent");
		//	return MS::kFailure;
		//}

		// resource is not release before the handle is closed
		//CloseHandle(handleThreadGL);
		//handleThreadGL = NULL;
	}
	else
	{
		handleThreadGL = (HANDLE)_beginthreadex(0, 0, threadProcGL, &threadDataGL, 0, 0);
	}

	return MS::kSuccess;
}