#ifndef CONTROL_MESH_H
#define CONTROL_MESH_H

#include <memory>
#include <maya/MDagPath.h>
#include <maya/MImage.h>

#include "CCLevel.h"


class ControlMesh
{
public:
	bool levelsGenerated;
	int maxSubdivisionLevel;
	int numVerticesAllLevels;
	std::vector<std::shared_ptr<CCLevel> > levels; // levels[0] is unsubdivided
	std::vector<float> verticesRawShared; // shared by all CCLevels

	bool isGLSetup;
	GLuint vbo;
	GLuint fpProgram, epProgram, vpProgram;

	// Textures
	bool hasDisplacementMap, hasTexture;
	MImage dispMap, texMap;
	GLuint diffuseMap, displacementMap;

	ControlMesh() :
		maxSubdivisionLevel(6), isGLSetup(false), levelsGenerated(false), debugBuffersGenerated(false),
		hasDisplacementMap(false), hasTexture(false), diffuseMap(std::numeric_limits<GLuint>::max()),
		displacementMap(std::numeric_limits<GLuint>::max()) {}
	virtual ~ControlMesh();

	MStatus initBaseMeshFromMaya(MDagPath meshDagPath);

	void adaptiveCCAllLevels();

	// Need to do it separately because there is no context at the time initBaseMeshFromMaya is called
	void generateGLTextures();

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