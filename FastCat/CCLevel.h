#ifndef CCLEVEL_H
#define CCLEVEL_H

#include "CommonInclude.h"
#include "MeshUtility.h"

#include <maya/MItMeshPolygon.h>
#include <maya/MPointArray.h>
#include <unordered_map>
#include <set>


class CCLevel
{
public:
	std::vector<Vertex> vlist;
	std::vector<Edge> elist;
	std::vector<Face> flist;

	bool isBaseLevel;
	bool patchesClassified;
	std::vector<Face *> endPatches;
	std::vector<Face *> fullPatchesNoSharp;
	std::vector<Face *> fullPatchesSharp;
	std::vector<Face *> partialPatchesNoSharp;
	std::vector<Face *> partialPatchesSharp;

	bool bufferGenerated;
	GLuint vao;
	int firstVertexOffset; // offset of the first vertex in the shared vertex bufffer

	GLuint fpProgram; // Computer shader programs used to compute positions of the vertices in next level
	GLuint epProgram;
	GLuint vpProgram;

	CCLevel();
	virtual ~CCLevel();

	void bindBuffers(GLuint vbo);

	// Initialize *this as a base level (unsubdivided mesh)
	// numVertices - number of vertices in the base level
	// itFace - iterator to the first face of the mesh
	void createBaseLevel(int numVertices, MItMeshPolygon &itFace);

	// Construct the next CCLevel and tables for computing vertex positions in the next level
	// New vertex positions are not computed after call to this method
	std::shared_ptr<CCLevel> adaptiveCatmullClark();

	// Mark the end patches (patches around features). No actual subdivision is done in this method.
	// Only use it with the last subdivision level.
	void markEndPatches();

	// Compute vertex positions for next level by dispatching corresponding CSs
	void runSubdivisionTables(GLuint vbo);

	// Classify the faces in flist according what types of patches they are
	// and put them into different collections.
	// Only call this method after calling adaptiveCatmullClark() or markEndPatches()
	void classifyPatches();

private:
	std::unordered_map<unsigned, std::unordered_map<unsigned, Edge *> > lut;

	// Tables for subdivision on compute shaders
	GLuint tableBuffers[7];
	// Face tables
	int f_numPoints;
	std::vector<int> f_offsetValenceTable;
	std::vector<int> f_neighbourIndexTable;
	// Edge tables
	int e_numPoints;
	std::vector<int> e_neighbourIndexTable;
	std::vector<float> e_sharpnessTable;
	// VertexTables
	int v_numPoints;
	std::vector<int> v_ovpc1c2Table; // ovpc1c2 = offset + valence + parent index + crease1 index + crease2 index
	std::vector<int> v_neighbourIndexTable;
	std::vector<float> v_sharpnessTable;

	// faceVertexIndices store indices into vlist (not indices into shared vertex buffer)
	Face *addFace(const std::vector<unsigned> &faceVertexIndices);

	// Mark face to be subdivided
	// Put face and its one-ring neighbor into subd_faces
	// The vertices and edges of the faces in subd_faces are put into subd_vertices and subd_edges, respectively
	// A Edge is not put into subd_edges if its dual is already there
	// Also mark neighbouring faces if markNeighbours == true
	void markSubdivideFace(Face *face, std::set<Face *> &subd_faces, std::set<Edge *> &subd_edges,
						   std::set<Vertex *> &subd_vertices, bool markNeighbours = false);

	// Subdivide @parent using CC rule and add the resulting faces, edges into this level.
	// Update vertices (i.e. valence, edge pointer) accordingly.
	void addSubFaces(Face *parent, int edgePointStart, int vertexPointStart);

	// Put the indices of @face's vertices into @indices
	// @indices need to be resized before calling
	void appendFaceVertexIndices(int offset, Face *face, std::vector<int> &indices);
	
	// Put the indices of vertices needed to compute the edge point into @indices
	// @indices need to be resized before calling
	void appendEdgeVertexIndices(int offset, Edge *edge, std::vector<int> &indices);
	
	// Update tables according to the configuration of @vertex (i.e. corner, boundary, crease, dart, interior)
	// Don't resize tables beforehead
	void updateVertexPointTables(Vertex *vertex, std::vector<int> &ovpc1c2Table, std::vector<int> &neighbourIndexTable,
								 std::vector<float> &sharpnessTable);

	// set constants used by CSs
	void setCSUniforms(GLuint program);
};

#endif // CCLEVEL_H