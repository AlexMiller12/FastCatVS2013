#include "CCLevel.h"
#include "ShaderHelper.h"


void CCLevel::runSubdivisionTables(GLuint vbo)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo); // shared vertex buffer (4-float positions)
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tableBuffers[0]); // f_offsetValenceTable
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tableBuffers[1]); // f_neighbourIndexTable
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tableBuffers[2]); // e_neighbourIndexTable
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, tableBuffers[3]); // v_ovpc1c2Table
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, tableBuffers[4]); // v_neighbourIndexTable
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, tableBuffers[5]); // e_sharpnessTable
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, tableBuffers[6]); // v_sharpnessTable

	// compute face points
	glUseProgram(fpProgram);
	setCSUniforms(fpProgram);
	int numGroups = (f_numPoints + 63) / 64;
	glDispatchCompute(numGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glFinish();

	// compute edge points
	glUseProgram(epProgram);
	setCSUniforms(epProgram);
	numGroups = (e_numPoints + 63) / 64;
	glDispatchCompute(numGroups, 1, 1);

	// compute vertex points
	glUseProgram(vpProgram);
	setCSUniforms(vpProgram);
	numGroups = (e_numPoints + 7) / 8;
	glDispatchCompute(numGroups, 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glFinish();
}


void CCLevel::setCSUniforms(GLuint program)
{
	GLint numFacePointsLoc = ShaderHelper::getUniformLocation(program, "numFacePoints", true);
	GLint numEdgePointsLoc = ShaderHelper::getUniformLocation(program, "numEdgePoints", true);
	GLint numVertexPointsLoc = ShaderHelper::getUniformLocation(program, "numVertexPoints", true);
	GLint destOffset1Loc = ShaderHelper::getUniformLocation(program, "destOffset1", true);
	GLint destOffset2Loc = ShaderHelper::getUniformLocation(program, "destOffset2", true);
	GLint destOffset3Loc = ShaderHelper::getUniformLocation(program, "destOffset3", true);
	GLint srcOffsetLoc = ShaderHelper::getUniformLocation(program, "srcOffset", true);

	int destOffset = firstVertexOffset + vlist.size();
	if (numFacePointsLoc >= 0)
		glUniform1i(numFacePointsLoc, f_numPoints);
	if (numEdgePointsLoc >= 0)
		glUniform1i(numEdgePointsLoc, e_numPoints);
	if (numVertexPointsLoc >= 0)
		glUniform1i(numVertexPointsLoc, v_numPoints);
	if (destOffset1Loc >= 0)
		glUniform1i(destOffset1Loc, destOffset);
	if (destOffset2Loc >= 0)
		glUniform1i(destOffset2Loc, destOffset + f_numPoints);
	if (destOffset3Loc >= 0)
		glUniform1i(destOffset3Loc, destOffset + f_numPoints + e_numPoints);
	if (srcOffsetLoc >= 0)
		glUniform1i(srcOffsetLoc, firstVertexOffset);
}


void CCLevel::appendFaceVertexIndices(int offset, Face *face, std::vector<int> &indices)
{
	int numVertices = face->valence;
	std::vector<Vertex *> vs;
	face->getVertices(vs);

	for (int i = 0; i < numVertices; ++i)
	{
		indices[offset + i] = static_cast<int>(vs[i]->idx); // indices are relative to firstVertexOffset
	}
}


void CCLevel::appendEdgeVertexIndices(int offset, Edge *edge, std::vector<int> &indices)
{
	if (edge->sharpness >= 1.0f || !edge->leftFace || !edge->rightFace ||
		edge->leftFace->newIdx < 0 || edge->rightFace->newIdx < 0) // boundary/sharp rule
	{
		indices[offset] = static_cast<int>(edge->origin->idx);
		indices[offset + 1] = static_cast<int>(edge->origin->idx);
		indices[offset + 2] = static_cast<int>(edge->dest->idx);
		indices[offset + 3] = static_cast<int>(edge->dest->idx);
	}
	else // smooth rule
	{
		indices[offset] = static_cast<int>(edge->origin->idx);
		indices[offset + 1] = vlist.size() + edge->leftFace->newIdx; // make it relative to firstVertexOffset
		indices[offset + 2] = static_cast<int>(edge->dest->idx);
		indices[offset + 3] = vlist.size() + edge->rightFace->newIdx;
	}
}


void CCLevel::updateVertexPointTables(Vertex *vertex,
							 std::vector<int> &ovpc1c2Table,
							 std::vector<int> &neighbourIndexTable,
							 std::vector<float> &sharpnessTable)
{
	// offset
	if (ovpc1c2Table.empty())
	{
		ovpc1c2Table.push_back(0);
	}
	else
	{
		int size = ovpc1c2Table.size();
		int preOffset = ovpc1c2Table[size - 5];
		int preValence = ovpc1c2Table[size - 4];

		if (preValence < 0)
		{
			ovpc1c2Table.push_back(preOffset);
		}
		else
		{
			ovpc1c2Table.push_back(preOffset + 2 * preValence);
		}
	}

	std::vector<int> neighbours, creases;
	int numCreases = 0;
	float averageSharpness = 0.f;
	int n = vertex->getOneRingMeta(neighbours, &numCreases, &averageSharpness, creases);

	// valence
	ovpc1c2Table.push_back(n);
	// parent index
	ovpc1c2Table.push_back(vertex->idx);
	// c1 c2
	ovpc1c2Table.push_back(creases[0]);
	ovpc1c2Table.push_back(creases[1]);

	for (int i = 0; i < neighbours.size(); ++i)
	{
		if (i % 2 == 0)
		{
			neighbourIndexTable.push_back(neighbours[i]);
		}
		else
		{
			neighbourIndexTable.push_back(vlist.size() + neighbours[i]); // make it relative to firstVertexOffset
		}
	}
	sharpnessTable.push_back(averageSharpness);
}


void CCLevel::addSubFaces(Face *parent, int edgePointStart, int vertexPointStart)
{
	std::vector<Vertex *> vps;
	std::vector<Edge *> eps;

	parent->getEdges(eps);
	parent->getVertices(vps);
	assert(eps.size() == vps.size() && eps.size() == parent->valence);

	int numSubFaces = parent->valence;
	for (int i = 0; i < numSubFaces; ++i)
	{
		Edge *e1 = eps[(i + numSubFaces - 1) % numSubFaces];
		Edge *e2 = eps[i];
		int fpIdx = parent->newIdx; assert(fpIdx >= 0);
		int epIdx1 = edgePointStart + e1->newIdx; assert(epIdx1 >= 0);
		int epIdx2 = edgePointStart + e2->newIdx; assert(epIdx2 >= 0);
		int vpIdx = vertexPointStart + vps[i]->newIdx; assert(vpIdx >= 0);
		std::vector<unsigned> indices;
		indices.push_back(epIdx1);
		indices.push_back(vpIdx);
		indices.push_back(epIdx2);
		indices.push_back(fpIdx);

		Face *newFace = addFace(indices);

		// Update sharpness
		newFace->right->sharpness = fmaxf(0.f, e2->sharpness - 1.f);
		newFace->right->fPrev()->sharpness = fmaxf(0.f, e1->sharpness - 1.f);
		
			// Make sure subfaces oriented consistently
		for (int j = 0; j < i; ++j)
		{
			newFace->rotateCCW();
		}
	}
}


void CCLevel::markSubdivideFace(Face *face,
								std::set<Face *> &subd_faces,
								std::set<Edge *> &subd_edges,
								std::set<Vertex *> &subd_vertices,
								bool markNeighbours)
{
	std::set<Face *> sdfs;

	face->isMarkedForSubdivision = true;
	face->getOneRingNeighbourFaces(sdfs);

	if (markNeighbours)
	{
		std::set<Face *> sdfs_fix = sdfs;
		for (std::set<Face *>::iterator it = sdfs_fix.begin(); it != sdfs_fix.end(); ++it)
		{
			Face *f = *it;
			std::set<Face *> tmp_sdfs;
			
			if (!f->isMarkedForSubdivision)
			{
				f->isMarkedForSubdivision = true;
				f->getOneRingNeighbourFaces(tmp_sdfs);

				for (std::set<Face *>::iterator it2 = tmp_sdfs.begin(); it2 != tmp_sdfs.end(); ++it2)
				{
					sdfs.insert(*it2);
				}
			}
		}
	}

	for (std::set<Face *>::iterator it = sdfs.begin(); it != sdfs.end(); ++it)
	{
		Face *f = *it;
		
		std::set<Edge *> es;
		f->getEdges(es);
		for (std::set<Edge *>::iterator eit = es.begin(); eit != es.end(); ++eit)
		{
			if (!(*eit)->dual || subd_edges.find((*eit)->dual) == subd_edges.end())
			{
				subd_edges.insert(*eit);
			}
		}

		f->getVertices(subd_vertices);
	}

	for (std::set<Face *>::iterator it = sdfs.begin(); it != sdfs.end(); ++it)
	{
		subd_faces.insert(*it);
	}
}


std::shared_ptr<CCLevel> CCLevel::adaptiveCatmullClark()
{
	std::set<Face *> subd_faces; // faces to be subdivided
	std::set<Edge *> subd_edges;
	std::set<Vertex *> subd_vertices;

	// Subdivide due to non-quad or multiple creases
	for (int i = 0; i < flist.size(); ++i)
	{
		Face *f = &flist[i];

		if (f->valence != 4) // not a quad
		{
			// Neighbouring faces are marked because they don't have enough CPs
			// to be evaluated in the current level.
			// (The authours say that this can be done better)
			markSubdivideFace(f, subd_faces, subd_edges, subd_vertices, true);
		}

		if (f->numCreases() > 1)
		{
			markSubdivideFace(f, subd_faces, subd_edges, subd_vertices);
		}
	}

	// Subdivide due to unevaluable creases
	for (int i = 0; i < elist.size(); ++i)
	{
		Edge *e = &elist[i];
		if (e->sharpness > 0.f)
		{
			if (!e->origin->isTagged && e->origin->isUnevaluable())
			{
				e->origin->isTagged = true;
			}
			if (!e->dest->isTagged && e->dest->isUnevaluable())
			{
				e->dest->isTagged = true;
			}
		}
	}

	// Subdivide due to extraordinary vertices
	for (int i = 0; i < vlist.size(); ++i)
	{
		Vertex *v = &vlist[i];
		if (v->valence > 0 && v->valence != 4) // ignore boundary
		{
			v->isTagged = true;
		}
	}

	// Mark and subdivide all faces connecting to a tagged vertex
	for (int i = 0; i < vlist.size(); ++i)
	{
		Vertex *v = &vlist[i];
		if (v->isTagged)
		{
			Edge *cur = v->edge;
			do
			{
				markSubdivideFace(cur->leftFace, subd_faces, subd_edges, subd_vertices);
				cur = cur->vNext();
			} while (cur != v->edge);
		}
	}

	std::shared_ptr<CCLevel> nextLevel = std::make_shared<CCLevel>();
	nextLevel->firstVertexOffset = firstVertexOffset + vlist.size();
	int numFacesNextLevel = 0;
	int numFacePoints = subd_faces.size(), numEdgePoints = subd_edges.size(), numVertexPoints = subd_vertices.size();

	// record for later use
	f_numPoints = numFacePoints;
	e_numPoints = numEdgePoints;
	v_numPoints = numVertexPoints;
	
	int i = 0;
	std::vector<Face *> subd_faces_linear;
	for (std::set<Face *>::iterator it = subd_faces.begin(); it != subd_faces.end(); ++it, ++i)
	{
		Face *f = *it;
		f->newIdx = i;
		subd_faces_linear.push_back(f);
		numFacesNextLevel += f->valence;
	}

	i = 0;
	std::vector<Edge *> subd_edges_linear;
	for (std::set<Edge *>::iterator it = subd_edges.begin(); it != subd_edges.end(); ++it, ++i)
	{
		Edge *e = *it;
		e->newIdx = i;
		if (e->dual)
		{
			e->dual->newIdx = i;
		}
		subd_edges_linear.push_back(e);
	}

	i = 0;
	std::vector<Vertex *> subd_vertices_linear;
	for (std::set<Vertex *>::iterator it = subd_vertices.begin(); it != subd_vertices.end(); ++it, ++i)
	{
		Vertex *v = *it;
		v->newIdx = i;
		subd_vertices_linear.push_back(v);
	}

	// Construct next CCLevel
	int numVerticesNextLevel = numFacePoints + numEdgePoints + numVertexPoints;
	nextLevel->flist.reserve(numFacesNextLevel);
	nextLevel->elist.reserve(4 * numFacesNextLevel); // each face will be a quad after one level of subdivision
	nextLevel->vlist.resize(numVerticesNextLevel);

	for (int i = 0; i < numVerticesNextLevel; ++i)
	{
		nextLevel->vlist[i].idx = i;
	}

	nextLevel->lut.clear();
	for (int i = 0; i < subd_faces_linear.size(); ++i)
	{
		Face *f = subd_faces_linear[i];
		nextLevel->addSubFaces(f, numFacePoints, numFacePoints + numEdgePoints);
	}
	nextLevel->lut.clear();

	for (int i = 0; i < nextLevel->elist.size(); ++i)
	{
		Edge *e = &nextLevel->elist[i];
		if (!e->dual)
		{
			if (e->origin->valence > 0)
			{
				e->origin->valence *= -1;
			}
			if (e->dest->valence > 0)
			{
				e->dest->valence *= -1;
			}
		}
	}

	// Build face tables
	f_offsetValenceTable.resize(2 * numFacePoints);
	f_neighbourIndexTable.resize(numFacesNextLevel);

	for (int i = 0; i < numFacePoints; ++i)
	{
		Face *f = subd_faces_linear[i];

		if (i == 0)
		{
			f_offsetValenceTable[0] = 0;
		}
		else
		{
			f_offsetValenceTable[2 * i] = f_offsetValenceTable[2 * (i - 1)] + f_offsetValenceTable[2 * (i - 1) + 1];
		}

		f_offsetValenceTable[2 * i + 1] = f->valence;
		appendFaceVertexIndices(f_offsetValenceTable[2 * i], f, f_neighbourIndexTable);
	}

	// Build edge tables
	e_sharpnessTable.resize(numEdgePoints);
	e_neighbourIndexTable.resize(4 * numEdgePoints);

	for (int i = 0; i < numEdgePoints; ++i)
	{
		Edge *e = subd_edges_linear[i];
		e_sharpnessTable[i] = e->sharpness;
		appendEdgeVertexIndices(4 * i, e, e_neighbourIndexTable);
	}

	// Build vertex tables
	for (int i = 0; i < numVertexPoints; ++i)
	{
		Vertex *v = subd_vertices_linear[i];
		updateVertexPointTables(v, v_ovpc1c2Table, v_neighbourIndexTable, v_sharpnessTable);
	}

	// Create buffers and load data
	glGenBuffers(7, tableBuffers);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, f_offsetValenceTable.size() * sizeof(int), f_offsetValenceTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, f_neighbourIndexTable.size() * sizeof(int), f_neighbourIndexTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[2]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, e_neighbourIndexTable.size() * sizeof(int), e_neighbourIndexTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[3]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, v_ovpc1c2Table.size() * sizeof(int), v_ovpc1c2Table.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[4]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, v_neighbourIndexTable.size() * sizeof(int), v_neighbourIndexTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[5]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, e_sharpnessTable.size() * sizeof(float), e_sharpnessTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tableBuffers[6]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, v_sharpnessTable.size() * sizeof(float), v_sharpnessTable.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

	return nextLevel;
}


void CCLevel::bindBuffers(GLuint vbo)
{
	if (!bufferGenerated)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Vertex buffer is shared by all CCLevels of the same mesh
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(firstVertexOffset * 4 * sizeof(float)));
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


Face *CCLevel::addFace(const std::vector<unsigned> &faceVertexIndices)
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

	return f;
}