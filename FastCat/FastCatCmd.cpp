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

	MStatus toMeshStatus;
	MFnMesh fnMesh(node, &toMeshStatus); // Fn stands for a function set

	if (MS::kSuccess != toMeshStatus)
	{
		MGlobal::displayError("Selection is not a polymesh");
		return MS::kFailure;
	}

	// Get vertex positions
	std::shared_ptr<CCLevel> level = std::make_shared<CCLevel>();
	MPointArray vertices;
	fnMesh.getPoints(vertices); // in object space
	MFloatVectorArray normals;
	fnMesh.getNormals(normals);

	// TODO: get UVs for displacement mapping

	// Get the indices of face vertices
	MItMeshPolygon itFace(node);
	while (!itFace.isDone())
	{
		int nv = itFace.polygonVertexCount(); // number of vertices
		if (nv == 4)
		{
			int vidx, nidx;
			for (int i = 0; i < 3; ++i)
			{
				vidx = itFace.vertexIndex(i % nv);
				nidx = itFace.normalIndex(i % nv);
				level->vertexBuffer.push_back(vertices[vidx].x);
				level->vertexBuffer.push_back(vertices[vidx].y);
				level->vertexBuffer.push_back(vertices[vidx].z);
				level->vertexBuffer.push_back(normals[nidx].x);
				level->vertexBuffer.push_back(normals[nidx].y);
				level->vertexBuffer.push_back(normals[nidx].z);
			}
			for (int i = 2; i < 5; ++i)
			{
				vidx = itFace.vertexIndex(i % nv);
				nidx = itFace.normalIndex(i % nv);
				level->vertexBuffer.push_back(vertices[vidx].x);
				level->vertexBuffer.push_back(vertices[vidx].y);
				level->vertexBuffer.push_back(vertices[vidx].z);
				level->vertexBuffer.push_back(normals[nidx].x);
				level->vertexBuffer.push_back(normals[nidx].y);
				level->vertexBuffer.push_back(normals[nidx].z);
			}
		}
		else if (nv == 3)
		{
			int vidx, nidx;
			for (int i = 0; i < 3; ++i)
			{
				vidx = itFace.vertexIndex(i % nv);
				nidx = itFace.normalIndex(i % nv);
				level->vertexBuffer.push_back(vertices[vidx].x);
				level->vertexBuffer.push_back(vertices[vidx].y);
				level->vertexBuffer.push_back(vertices[vidx].z);
				level->vertexBuffer.push_back(normals[nidx].x);
				level->vertexBuffer.push_back(normals[nidx].y);
				level->vertexBuffer.push_back(normals[nidx].z);
			}
		}

		itFace.next();
	}

	testRenderer.testMesh = level;
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