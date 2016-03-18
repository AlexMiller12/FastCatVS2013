#include "CCLevel.h"


void CCLevel::bindBuffers(GLuint vbo)
{
	if (!bufferGenerated)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Vertex buffer is shared by all CCLevels of the same mesh
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)(firstVertexOffset * 3 * sizeof(float)));
		bufferGenerated = true;
	}
	else
	{
		glBindVertexArray(vao);
	}
}


void CCLevel::createBaseLevel(int numVertices, MItMeshPolygon itFace)
{
	lut.clear();
	vlist.clear();
	elist.clear();
	flist.clear();
	firstVertexOffset = 0;

	for (int i = 0; i < numVertices; ++i)
	{
		Vertex v;
		v.idx = i;
		vlist.push_back(v);
	}
	
	int ne = 0, nf = 0;
	while (!itFace.isDone())
	{
		ne += itFace.polygonVertexCount();
		++nf;
		itFace.next();
	}
	itFace.reset();
	elist.reserve(ne); // prevent reallocation which will invalidate pointers
	flist.reserve(nf);

	while (!itFace.isDone())
	{
		int nv = itFace.polygonVertexCount();
		std::vector<unsigned> indices; // in CCW order
		
		for (int i = 0; i < nv; ++i)
		{
			indices.push_back(itFace.vertexIndex(i));
		}
		
		// Create face and corresponding (half) edges
		addFace(indices);

		itFace.next();
	}

	for (int i = 0; i < elist.size(); ++i)
	{
		Edge &e = elist[i];

		// boundary
		if (!e.dual)
		{
			if (e.origin->valence > 0)
			{
				e.origin->valence *= -1;
			}
			if (e.dest->valence > 0)
			{
				e.dest->valence *= -1;
			}
		}
	}

	lut.clear();
}


void CCLevel::addFace(const std::vector<unsigned> &faceVertexIndices)
{
	int startIdx = elist.size();
	int numEdges = faceVertexIndices.size();

	elist.resize(startIdx + numEdges);
	Face newFace;
	newFace.valence = numEdges;
	newFace.right = &elist[startIdx + 1];
	flist.push_back(newFace);
	Face *f = &flist.back();

	for (int i = 0; i < numEdges; ++i)
	{
		int ei = startIdx + i;
		Edge *e = &elist[ei];
		Vertex *org = &vlist[faceVertexIndices[i]];
		Vertex *dest = &vlist[faceVertexIndices[(i + 1) % numEdges]];
		Edge *next = &elist[startIdx + (i + 1) % numEdges];
		Edge *prev = &elist[startIdx + (i + numEdges - 1) % numEdges];

		if (!org->edge)
		{
			org->edge = e;
		}
		++org->valence;

		e->origin = org;
		e->dest = dest;
		e->leftFace = f;
		e->_next = next;
		e->_prev = prev;

		// check if the dual edge exist
		auto itResult1 = lut.find(dest->idx);
		if (itResult1 != lut.end()) // found edge(s) start from dest
		{
			auto itResult2 = itResult1->second.find(org->idx);
			if (itResult2 != itResult1->second.end()) // found the dual
			{
				Edge *dual = itResult2->second;

				e->dual = dual;
				e->rightFace = dual->leftFace;
				dual->dual = e;
				dual->rightFace = f;
			}
		}

		lut[org->idx][dest->idx] = e;
	}
}