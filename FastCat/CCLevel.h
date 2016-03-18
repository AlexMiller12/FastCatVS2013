#ifndef CCLEVEL_H
#define CCLEVEL_H

#include "CommonInclude.h"
#include "MeshUtility.h"

#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <unordered_map>

class CCLevel
{
public:
	CCLevel() : bufferGenerated(false), firstVertexOffset(-1) {}
	virtual ~CCLevel() {}

	void bindBuffers(GLuint vbo);

	// Initialize *this as a base level (unsubdivided mesh)
	// numVertices - number of vertices in the base level
	// itFace - iterator to the first face of the mesh
	void createBaseLevel(int numVertices, MItMeshPolygon itFace);

	std::vector<Vertex> vlist;
	std::vector<Edge> elist;
	std::vector<Face> flist;

	bool bufferGenerated;
	GLuint vao;
	int firstVertexOffset; // offset of the first vertex in the shared vertex bufffer

private:
	std::unordered_map<unsigned, std::unordered_map<unsigned, Edge *> > lut;

	// faceVertexIndices store indices into vlist (not indices into shared vertex buffer)
	void addFace(const std::vector<unsigned> &faceVertexIndices);
};

#endif // CCLEVEL_H