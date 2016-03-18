#ifndef MESH_UTILITY_H
#define MESH_UTILITY_H

#include <vector>

class Edge;
class Face;

class Vertex
{
public:
	unsigned idx; // offset into the shared vertex buffer
	int valence = 0;
	Edge *edge = NULL; // one of the edges going out from this vertex
};


// Actually half-edge
class Edge
{
public:
	float sharpness = 0.f;
	bool isTriangleHead = false;

	Vertex *origin = NULL;
	Vertex *dest = NULL;

	inline Vertex *diag() { return _next->dest; }

	Face *leftFace = NULL;
	Face *rightFace = NULL;
	
	Edge *dual = NULL;
	
	// Used to visit every edge of the face this edge belongs to
	inline Edge *fNext() { return _next; }
	inline Edge *fPrev() { return _prev; }
	
	// Used to visit every outgoing edge of origin
	inline Edge *vNext() { return _prev->dual; }
	inline Edge *vPrev() { return dual->_next; }

	// Don't use these two directly
	Edge *_next = NULL;
	Edge *_prev = NULL;
};


class Face
{
public:
	int valence = 0;
	bool isMarkedForSubdivision = false;
	Edge *right = NULL; // Edge on the right side of the face
};

#endif // MESH_UTILITY_H