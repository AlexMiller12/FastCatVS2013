#include "ControlMesh.h"
#include "ShaderHelper.h"

#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>


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
					 GL_DYNAMIC_READ); // make it READ for debugging. Use COPY when done

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

	// for debugging only
	glBindBuffer(GL_COPY_READ_BUFFER, vbo);
	float *mapped = reinterpret_cast<float *>(
		glMapBufferRange(GL_COPY_READ_BUFFER, 0, verticesRawShared.size() * sizeof(float), GL_MAP_READ_BIT)
		);
	std::memcpy(&verticesRawShared[0], mapped, verticesRawShared.size() * sizeof(float));
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
	fnMesh.getPoints(vertices); // in object space
	int numVertices = vertices.length();
	
	for (int i = 0; i < numVertices; ++i)
	{
		verticesRawShared.push_back(vertices[i].x);
		verticesRawShared.push_back(vertices[i].y);
		verticesRawShared.push_back(vertices[i].z);
		verticesRawShared.push_back(1.f);
	}

	// TODO: get UVs for displacement mapping

	std::shared_ptr<CCLevel> level = std::make_shared<CCLevel>();
	levels.push_back(level);
	MItMeshPolygon itFace(shapeNode);

	level->createBaseLevel(numVertices, itFace);

	return MS::kSuccess;
}


void ControlMesh::createDebugBuffers()
{
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
					debugVertexBuffer.push_back(faceNormal.x);
					debugVertexBuffer.push_back(faceNormal.y);
					debugVertexBuffer.push_back(faceNormal.z);
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
					debugVertexBuffer.push_back(faceNormal.x);
					debugVertexBuffer.push_back(faceNormal.y);
					debugVertexBuffer.push_back(faceNormal.z);
				}
			}
		}
	}

	offsetDebug.resize(numSubdivisions);
	offsetDebug[0] = 0;
	for (int i = 1; i < numSubdivisions; ++i)
	{
		offsetDebug[i] = offsetDebug[i - 1] + numVerticesDebug[i - 1] * 6 * sizeof(float);
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
		createDebugBuffers();
	}

	int offs = getOffsetDebug(level);

	glBindVertexArray(debugVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)offs);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)(offs + 3 * sizeof(float)));
}


void ControlMesh::clearDebugBuffers()
{
	glDeleteBuffers(1, &debugVBO);
	glDeleteVertexArrays(1, &debugVAO);
	debugVertexBuffer.clear();
	numVerticesDebug.clear();
	offsetDebug.clear();
	debugBuffersGenerated = false;
}