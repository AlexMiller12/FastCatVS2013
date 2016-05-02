#include "ControlMesh.h"
#include "ShaderHelper.h"

#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MUintArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MFloatArray.h>
#include <maya/MStringArray.h>


ControlMesh::~ControlMesh()
{
	if (isGLSetup)
	{
		glDeleteBuffers(1, &vbo);
		ShaderHelper::deleteProgram(fpProgram);
		ShaderHelper::deleteProgram(epProgram);
		ShaderHelper::deleteProgram(vpProgram);
	}
}


void ControlMesh::adaptiveCCAllLevels()
{
	if (!levelsGenerated)
	{
		if (levels.size() != 1)
		{
			MGlobal::displayError("Only base level is expected");
			return;
		}

		for (int i = 0; i < maxSubdivisionLevel; ++i)
		{
			std::shared_ptr<CCLevel> level = levels[i]->adaptiveCatmullClark();
			levels.push_back(level);
		}
		levels.back()->markEndPatches();
		levels.back()->isLastLevel = true;

		for (int i = 0; i <= maxSubdivisionLevel; ++i)
		{
			levels[i]->classifyPatches();
		}

		levelsGenerated = true;
		numVerticesAllLevels = levels.back()->firstVertexOffset + levels.back()->vlist.size();
		verticesRawShared.resize(numVerticesAllLevels * 4);
	}

	if (!isGLSetup)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER,
					 verticesRawShared.size() * sizeof(float),
					 verticesRawShared.data(),
					 GL_DYNAMIC_COPY); // make it READ for debugging. Use COPY when done

		std::string fpcsFileName(SHADER_DIR);
		std::string epcsFileName(SHADER_DIR);
		std::string vpcsFileName(SHADER_DIR);
		fpcsFileName += "/facepoint_cs.glsl";
		epcsFileName += "/edgepoint_cs.glsl";
		vpcsFileName += "/vertexpoint_cs.glsl";
		std::vector<GLenum> types(1);
		std::vector<const char *> fileNames(1);
		
		types[0] = GL_COMPUTE_SHADER;
		fileNames[0] = fpcsFileName.c_str();
		ShaderHelper::createProgramWithShaders(types, fileNames, fpProgram);

		fileNames[0] = epcsFileName.c_str();
		ShaderHelper::createProgramWithShaders(types, fileNames, epProgram);

		fileNames[0] = vpcsFileName.c_str();
		ShaderHelper::createProgramWithShaders(types, fileNames, vpProgram);

		for (int i = 0; i < maxSubdivisionLevel + 1; ++i)
		{
			levels[i]->fpProgram = fpProgram;
			levels[i]->epProgram = epProgram;
			levels[i]->vpProgram = vpProgram;
		}

		isGLSetup = true;
	}

	for (int i = 0; i < maxSubdivisionLevel; ++i)
	{
		levels[i]->runSubdivisionTables(vbo);
	}

#ifdef FAST_CAT_DEBUG_MODE
	glBindBuffer(GL_COPY_READ_BUFFER, vbo);
	float *mapped = reinterpret_cast<float *>(
		glMapBufferRange(GL_COPY_READ_BUFFER, 0, verticesRawShared.size() * sizeof(float), GL_MAP_READ_BIT)
		);
	std::memcpy(&verticesRawShared[0], mapped, verticesRawShared.size() * sizeof(float));
	glUnmapBuffer(GL_COPY_READ_BUFFER);
#endif
}


MStatus ControlMesh::initBaseMeshFromMaya(MObject shapeNode)
{
	MStatus toMeshStatus;
	MFnMesh fnMesh(shapeNode, &toMeshStatus); // Fn stands for a function set

	if (MS::kSuccess != toMeshStatus)
	{
		MGlobal::displayError("Selection is not a polymesh");
		return MS::kFailure;
	}

	// Pack data into vertex buffer
	MPointArray vertices;
	MFloatArray uTexCoords, vTexCoords;
	MStringArray uvSetNames;

	fnMesh.getUVSetNames(uvSetNames);
	int numUVSets = uvSetNames.length();
	bool hasUVs = numUVSets > 0 && fnMesh.numUVs(uvSetNames[0]) > 0;

	if (numUVSets > 1)
	{
		MGlobal::displayError("Only one UV set is supported right now");
		return MS::kFailure;
	}

	fnMesh.getPoints(vertices); // in object space
	int numVertices = vertices.length();
	
	for (int i = 0; i < numVertices; ++i)
	{
		verticesRawShared.push_back(vertices[i].x);
		verticesRawShared.push_back(vertices[i].y);
		verticesRawShared.push_back(vertices[i].z);
		verticesRawShared.push_back(1.f);
	}

	fnMesh.getUVs(uTexCoords, vTexCoords, &uvSetNames[0]);
	int numUVs = uTexCoords.length();
	std::vector<float> vertexUVs;

	for (int i = 0; i < numUVs; ++i)
	{
		vertexUVs.push_back(uTexCoords[i]);
		vertexUVs.push_back(vTexCoords[i]);
	}

	// Get creases and their sharpness
	MUintArray eids;
	MDoubleArray sharpnesses;
	EdgeSharpnessLUT edgeSharpnessLUT; // <vertex ID 1, vertex ID 2> -> sharpness

	fnMesh.getCreaseEdges(eids, sharpnesses);

	for (int i = 0; i < eids.length(); ++i)
	{
		int2 vids;
		fnMesh.getEdgeVertices(eids[i], vids);
		edgeSharpnessLUT[std::pair<int, int>(vids[0], vids[1])] = sharpnesses[i];
		edgeSharpnessLUT[std::pair<int, int>(vids[1], vids[0])] = sharpnesses[i];
	}

	// TODO: get UVs for displacement mapping

	std::shared_ptr<CCLevel> level = std::make_shared<CCLevel>();
	levels.push_back(level);
	MItMeshPolygon itFace(shapeNode);

	level->createBaseLevel(numVertices, itFace,
		                   edgeSharpnessLUT.empty()? NULL : &edgeSharpnessLUT,
						   hasUVs? &vertexUVs : NULL);

	return MS::kSuccess;
}


void ControlMesh::createPatchTypeDebugBuffers()
{
	auto addPNC = // add position, normal, and color
		[this](const glm::vec3 &position, glm::vec3 &normal, const glm::vec3 &color)
	{
		debugVertexBuffer.push_back(position.x);
		debugVertexBuffer.push_back(position.y);
		debugVertexBuffer.push_back(position.z);
		debugVertexBuffer.push_back(normal.x);
		debugVertexBuffer.push_back(normal.y);
		debugVertexBuffer.push_back(normal.z);
		debugVertexBuffer.push_back(color.r);
		debugVertexBuffer.push_back(color.g);
		debugVertexBuffer.push_back(color.b);
	};

	auto addFaceToDebugBuffer =
		[this, addPNC](Face *f, const glm::vec3 &color, int vertexStart, int level)
	{
		int nv = f->valence;
		Edge *e = f->right;

		if (nv == 4)
		{
			numVerticesDebug[level] += 6;

			unsigned vidx1 = vertexStart + e->origin->idx;
			e = e->fNext();
			unsigned vidx2 = vertexStart + e->origin->idx;
			e = e->fNext();
			unsigned vidx3 = vertexStart + e->origin->idx;
			e = e->fNext();
			unsigned vidx4 = vertexStart + e->origin->idx;

			glm::vec3 vs[6];
			vs[0] = glm::vec3(verticesRawShared[4 * vidx1], verticesRawShared[4 * vidx1 + 1], verticesRawShared[4 * vidx1 + 2]);
			vs[1] = glm::vec3(verticesRawShared[4 * vidx2], verticesRawShared[4 * vidx2 + 1], verticesRawShared[4 * vidx2 + 2]);
			vs[2] = glm::vec3(verticesRawShared[4 * vidx3], verticesRawShared[4 * vidx3 + 1], verticesRawShared[4 * vidx3 + 2]);
			vs[4] = glm::vec3(verticesRawShared[4 * vidx4], verticesRawShared[4 * vidx4 + 1], verticesRawShared[4 * vidx4 + 2]);
			vs[3] = vs[2];
			vs[5] = vs[0];
			glm::vec3 e12 = vs[1] - vs[0];
			glm::vec3 e23 = vs[2] - vs[1];
			glm::vec3 faceNormal = glm::normalize(glm::cross(e12, e23));

			for (int k = 0; k < 6; ++k)
			{
				addPNC(vs[k], faceNormal, color);
			}
		}
		else if (nv == 3)
		{
			numVerticesDebug[level] += 3;

			unsigned vidx1 = vertexStart + e->origin->idx;
			e = e->fNext();
			unsigned vidx2 = vertexStart + e->origin->idx;
			e = e->fNext();
			unsigned vidx3 = vertexStart + e->origin->idx;

			glm::vec3 vs[3];
			vs[0] = glm::vec3(verticesRawShared[4 * vidx1], verticesRawShared[4 * vidx1 + 1], verticesRawShared[4 * vidx1 + 2]);
			vs[1] = glm::vec3(verticesRawShared[4 * vidx2], verticesRawShared[4 * vidx2 + 1], verticesRawShared[4 * vidx2 + 2]);
			vs[2] = glm::vec3(verticesRawShared[4 * vidx3], verticesRawShared[4 * vidx3 + 1], verticesRawShared[4 * vidx3 + 2]);
			glm::vec3 e12 = vs[1] - vs[0];
			glm::vec3 e23 = vs[2] - vs[1];
			glm::vec3 faceNormal = glm::normalize(glm::cross(e12, e23));

			for (int k = 0; k < 3; ++k)
			{
				addPNC(vs[k], faceNormal, color);
			}
		}
	};

	debugVertexBuffer.clear();
	numVerticesDebug.clear();
	int numSubdivisions = levels.size();

	for (int i = 0; i < numSubdivisions; ++i)
	{
		int vertexStart = levels[i]->firstVertexOffset;
		std::vector<Face *> &endPatches = levels[i]->endPatches;
		std::vector<Face *> &fullPatchesNoSharp = levels[i]->fullPatchesNoSharp;
		std::vector<Face *> &fullPatchesSharp = levels[i]->fullPatchesSharp;
		std::vector<Face *> &partialPatchesNoSharp = levels[i]->partialPatchesNoSharp;
		std::vector<Face *> &partialPatchesSharp = levels[i]->partialPatchesSharp;
		numVerticesDebug.push_back(0);

		for (int j = 0; j < endPatches.size(); ++j)
		{
			Face *f = endPatches[j];
			addFaceToDebugBuffer(f, glm::vec3(0.f, 1.f, 1.f), vertexStart, i);
		}

		for (int j = 0; j < fullPatchesNoSharp.size(); ++j)
		{
			Face *f = fullPatchesNoSharp[j];
			addFaceToDebugBuffer(f, glm::vec3(1.f, 1.f, 1.f), vertexStart, i);
		}

		for (int j = 0; j < fullPatchesSharp.size(); ++j)
		{
			Face *f = fullPatchesSharp[j];
			addFaceToDebugBuffer(f, glm::vec3(1.f, 0.f, 1.f), vertexStart, i);
		}

		for (int j = 0; j < partialPatchesNoSharp.size(); ++j)
		{
			Face *f = partialPatchesNoSharp[j];
			addFaceToDebugBuffer(f, glm::vec3(1.f, 1.f, 0.f), vertexStart, i);
		}

		for (int j = 0; j < partialPatchesSharp.size(); ++j)
		{
			Face *f = partialPatchesSharp[j];
			addFaceToDebugBuffer(f, glm::vec3(1.f, 0.f, 0.f), vertexStart, i);
		}
	}

	offsetDebug.resize(numSubdivisions);
	offsetDebug[0] = 0;
	for (int i = 1; i < numSubdivisions; ++i)
	{
		offsetDebug[i] = offsetDebug[i - 1] + numVerticesDebug[i - 1] * 9 * sizeof(float);
	}

	glGenVertexArrays(1, &debugVAO);
	glBindVertexArray(debugVAO);

	// Vertex buffer is shared by all CCLevels of the same mesh
	glGenBuffers(1, &debugVBO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, debugVertexBuffer.size() * sizeof(float), debugVertexBuffer.data(), GL_STATIC_DRAW);

	glBindVertexArray(0); // unbind vao
	debugBuffersGenerated = true;
}


void ControlMesh::createDebugBuffers()
{
	auto addNormalAndColor =
		[](std::vector<float> &dvb, const Face &f,
		   const glm::vec3 &normal, const glm::vec3 &c1, const glm::vec3 &c2, const glm::vec3 &c3)
	{
		dvb.push_back(normal.x);
		dvb.push_back(normal.y);
		dvb.push_back(normal.z);

		if (f.isMarkedForSubdivision)
		{
			dvb.push_back(c1.r);
			dvb.push_back(c1.g);
			dvb.push_back(c1.b);
		}
		else if (f.wasMarkedForSubdivision)
		{
			dvb.push_back(c2.r);
			dvb.push_back(c2.g);
			dvb.push_back(c2.b);
		}
		else
		{
			dvb.push_back(c3.r);
			dvb.push_back(c3.g);
			dvb.push_back(c3.b);
		}
	};

	debugVertexBuffer.clear();
	numVerticesDebug.clear();
	int numSubdivisions = levels.size();

	for (int i = 0; i < numSubdivisions; ++i)
	{
		int vertexStart = levels[i]->firstVertexOffset;
		std::vector<Face> &flist = levels[i]->flist;
		int numFaces = flist.size();
		numVerticesDebug.push_back(0);

		for (int j = 0; j < numFaces; ++j)
		{
			int nv = flist[j].valence;
			Edge *e = flist[j].right;

			if (nv == 4)
			{
				numVerticesDebug[i] += 6;
				
				unsigned vidx1 = vertexStart + e->origin->idx;
				e = e->fNext();
				unsigned vidx2 = vertexStart + e->origin->idx;
				e = e->fNext();
				unsigned vidx3 = vertexStart + e->origin->idx;
				e = e->fNext();
				unsigned vidx4 = vertexStart + e->origin->idx;

				glm::vec3 vs[6];
				vs[0] = glm::vec3(verticesRawShared[4 * vidx1], verticesRawShared[4 * vidx1 + 1], verticesRawShared[4 * vidx1 + 2]);
				vs[1] = glm::vec3(verticesRawShared[4 * vidx2], verticesRawShared[4 * vidx2 + 1], verticesRawShared[4 * vidx2 + 2]);
				vs[2] = glm::vec3(verticesRawShared[4 * vidx3], verticesRawShared[4 * vidx3 + 1], verticesRawShared[4 * vidx3 + 2]);
				vs[4] = glm::vec3(verticesRawShared[4 * vidx4], verticesRawShared[4 * vidx4 + 1], verticesRawShared[4 * vidx4 + 2]);
				vs[3] = vs[2];
				vs[5] = vs[0];
				glm::vec3 e12 = vs[1] - vs[0];
				glm::vec3 e23 = vs[2] - vs[1];
				glm::vec3 faceNormal = glm::normalize(glm::cross(e12, e23));

				for (int k = 0; k < 6; ++k)
				{
					debugVertexBuffer.push_back(vs[k].x);
					debugVertexBuffer.push_back(vs[k].y);
					debugVertexBuffer.push_back(vs[k].z);
					addNormalAndColor(debugVertexBuffer, flist[j], faceNormal,
									  glm::vec3(0.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
				}
			}
			else if (nv == 3)
			{
				numVerticesDebug[i] += 3;

				unsigned vidx1 = vertexStart + e->origin->idx;
				e = e->fNext();
				unsigned vidx2 = vertexStart + e->origin->idx;
				e = e->fNext();
				unsigned vidx3 = vertexStart + e->origin->idx;

				glm::vec3 vs[3];
				vs[0] = glm::vec3(verticesRawShared[4 * vidx1], verticesRawShared[4 * vidx1 + 1], verticesRawShared[4 * vidx1 + 2]);
				vs[1] = glm::vec3(verticesRawShared[4 * vidx2], verticesRawShared[4 * vidx2 + 1], verticesRawShared[4 * vidx2 + 2]);
				vs[2] = glm::vec3(verticesRawShared[4 * vidx3], verticesRawShared[4 * vidx3 + 1], verticesRawShared[4 * vidx3 + 2]);
				glm::vec3 e12 = vs[1] - vs[0];
				glm::vec3 e23 = vs[2] - vs[1];
				glm::vec3 faceNormal = glm::normalize(glm::cross(e12, e23));

				for (int k = 0; k < 3; ++k)
				{
					debugVertexBuffer.push_back(vs[k].x);
					debugVertexBuffer.push_back(vs[k].y);
					debugVertexBuffer.push_back(vs[k].z);
					addNormalAndColor(debugVertexBuffer, flist[j], faceNormal,
									  glm::vec3(0.f, 1.f, 1.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
				}
			}
		}
	}

	offsetDebug.resize(numSubdivisions);
	offsetDebug[0] = 0;
	for (int i = 1; i < numSubdivisions; ++i)
	{
		offsetDebug[i] = offsetDebug[i - 1] + numVerticesDebug[i - 1] * 9 * sizeof(float);
	}

	glGenVertexArrays(1, &debugVAO);
	glBindVertexArray(debugVAO);

	// Vertex buffer is shared by all CCLevels of the same mesh
	glGenBuffers(1, &debugVBO);
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, debugVertexBuffer.size() * sizeof(float), debugVertexBuffer.data(), GL_STATIC_DRAW);

	glBindVertexArray(0); // unbind vao
	debugBuffersGenerated = true;
}


MStatus ControlMesh::bindDebugBuffers(int level)
{
	if (level >= levels.size())
	{
		MGlobal::displayError("Subdivision level doesn't exist");
		return MS::kFailure;
	}

	if (!debugBuffersGenerated)
	{
		//createDebugBuffers();
		createPatchTypeDebugBuffers();
	}

	int offs = getOffsetDebug(level);

	glBindVertexArray(debugVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void *)offs);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void *)(offs + 3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void *)(offs + 6 * sizeof(float)));
}


void ControlMesh::clearDebugBuffers()
{
	if (debugBuffersGenerated)
	{
		glDeleteBuffers(1, &debugVBO);
		glDeleteVertexArrays(1, &debugVAO);
		debugVertexBuffer.clear();
		numVerticesDebug.clear();
		offsetDebug.clear();
		debugBuffersGenerated = false;
	}
}