#ifndef MESH_UTILITY_H
#define MESH_UTILITY_H

#include <vector>
#include <set>

class Edge;
class Face;

class Vertex
{
public:
	unsigned idx; // offset into the shared vertex buffer
	int newIdx = -1; // index of the corresponding point in the next subdivision level
	int valence = 0;
	bool isTagged = false; // is tagged for subdivision in the current level
	Edge *edge = NULL; // one of the edges going out from this vertex
	unsigned uvIndex;

	bool isUnevaluable();

	void getOneRingIndices(int firstVertexOffset, std::vector<int> &indices);

	void findBoundaryIndices(std::vector<int> &bis);

	// boundary (valence < -2) -> pure boundary rule
	//     -> neighbours: empty
	//     -> numCreases: 0
	//     -> averageSharpness: 0.f
	//     -> creases: contains the indices of the two vertices that form a boundary with the current vertex
	//     -> return valence (< -2)
	// corner (valence == -2)/some face's newIdx < 0 -> pure corner rule
	//     -> neighbours: empty
	//     -> numCreases: 0
	//     -> averageSharpness: 0.f
	//     -> creases: (-1, -1)
	//     -> return -2
	// valence > 0 && numCreases == 2 -> pure crease rule (sharpness >= 1.f)/blend between crease and interior rule (sharpness < 1.0f)
	//     -> neighbours: indices of neightbours (connected vertices and face points)
	//     -> numCreases: 2
	//     -> averageSharpness: literally
	//     -> creases: contains the indices of the two vertices that form the crease with the current vertex
	//     -> return valence (> 0)
	// valence > 0 && numCreases > 2 -> pure corner rule (sharpness >= 1.f)/blend between corner and interior rule (sharpness < 1.0f)
	//     -> neighbours: indices of neightbours (connected vertices and face points)
	//     -> numCreases: > 2
	//     -> averageSharpness: literally
	//     -> creases: (-1, -1)
	//     -> return valence (> 0)
	// valence > 0 && numCreases < 2 -> interior rule
	//     -> neighbours: indices of neightbours (connected vertices and face points)
	//     -> numCreases: < 2
	//     -> averageSharpness: 0.f
	//     -> creases: (-1, -1)
	//     -> return valence (> 0)
	int getOneRingMeta(std::vector<int> &neighbours, int *numCreases, float *averageSharpness, std::vector<int> &creases);
};


// Actually half-edge
class Edge
{
public:
	int newIdx = -1; // index to the face point in the next level
	float sharpness = 0.f;
	bool isTriangleHead = false; // true if this edge is in a face marked for subdivision

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
	inline Edge *vNext()
	{
		if (_prev->dual)
		{
			return _prev->dual;
		}
		else
		{
			Edge *e = this;
			while (e->dual)
			{
				e = e->dual->_next;
			}
			return e;
		}
	}
	inline Edge *vPrev()
	{
		if (dual)
		{
			return dual->_next;
		}
		else
		{
			Edge *e = this;
			while (e->_prev->dual)
			{
				e = e->_prev->dual;
			}
			return e;
		}
	}

	// Don't use these two directly
	Edge *_next = NULL;
	Edge *_prev = NULL;

	inline bool isSharp() { return sharpness > FLT_EPSILON; }
};


class Face
{
public:
	int valence = 0;
	Edge *right = NULL; // Edge on the right side of the face
	
	bool isMarkedForSubdivision = false;
	bool wasMarkedForSubdivision = false; // true if the parent of this face was marked
	int newIdx = -1;

	int numCreases();
	bool hasTriangleHead();
	void getPartialPatchInfo(int *p_numTriHeands, bool *p_connected) const;

	inline void rotateCCW() { right = right->fPrev(); }
	inline void rotateCW() { right = right->fNext(); }
	
	// Order is not important
	void getVertices(std::set<Vertex *> &vs);
	void getEdges(std::set<Edge *> &es);
	// If order is important, use these two
	void getVertices(std::vector<Vertex *> &vs);
	void getEdges(std::vector<Edge *> &es);

	// Includes self
	void getOneRingNeighbourFaces(std::set<Face *> &neighbours);

	// Get the 16 indices of control points
	// @outIndices need to be resized before calling this method
	// 12 13 14 15
	// 8  9  10 11
	// 4  5  6  7
	// 0  1  2  3
	void getOneRingIndices(int firstVertexOffset, unsigned *outIndices);
};

#endif // MESH_UTILITY_H