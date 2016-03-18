#ifndef CONTROL_MESH_H
#define CONTROL_MESH_H

#include "CCLevel.h"


class ControlMesh
{
public:
	std::vector<CCLevel> levels; // levels[0] is unsubdivided
	std::vector<float> verticesRawShared; // shared by all CCLevels

	ControlMesh() : debugBuffersGenerated(false) {}
	virtual ~ControlMesh() {}

	MStatus initBaseMeshFromMaya(MObject shapeNode);

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
};

#endif // CONTROL_MESH_H