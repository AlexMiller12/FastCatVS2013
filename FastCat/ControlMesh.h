#ifndef CONTROL_MESH_H
#define CONTROL_MESH_H

#include <memory>

#include "CCLevel.h"


class ControlMesh
{
public:
	bool levelsGenerated;
	int maxSubdivisionLevel;
	int numVerticesAllLevels;
	std::vector<std::shared_ptr<CCLevel> > levels; // levels[0] is unsubdivided
	std::vector<float> verticesRawShared; // shared by all CCLevels
	std::vector<float> vertexUVsShared;

	bool isGLSetup;
	GLuint vbo, texCoordBuffer;
	GLuint fpProgram, epProgram, vpProgram;

	ControlMesh() : maxSubdivisionLevel(6), isGLSetup(false), levelsGenerated(false), debugBuffersGenerated(false) {}
	virtual ~ControlMesh();

	MStatus initBaseMeshFromMaya(MObject shapeNode);

	void adaptiveCCAllLevels();


	// For debugging purpose
	bool debugBuffersGenerated;
	GLuint debugVAO, debugVBO;
	std::vector<float> debugVertexBuffer; // one vertex = 6 floats = (position, normal)
	std::vector<int> numVerticesDebug;
	std::vector<int> offsetDebug; // in bytes

	MStatus bindDebugBuffers(int level);
	void clearDebugBuffers();
	int getNumVerticesDebug(int level) { return (level < levels.size()) ? numVerticesDebug[level] : -1; }
	int getOffsetDebug(int level) { return (level < levels.size()) ? offsetDebug[level] : -1; }

private:
	// For debugging purpose
	void createDebugBuffers();
	void createPatchTypeDebugBuffers();
};

#endif // CONTROL_MESH_H