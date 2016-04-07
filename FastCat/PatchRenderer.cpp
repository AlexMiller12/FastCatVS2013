#include "PatchRenderer.h"
#include "ShaderHelper.h"


EndPatchRenderer::~EndPatchRenderer()
{
	if (indexBufferGenerated)
	{
		if (ibo != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ibo);
		}

		if (ovbo != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ovbo);
		}

		if (nibo != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &nibo);
		}
	}

	if (isGLSetup)
	{
		ShaderHelper::deleteProgram(program);
	}
}


void EndPatchRenderer::renderLevel(int level)
{
	// Only render end patches in the last subdivision level
	if (level == controlMesh->maxSubdivisionLevel && numIndices > 0)
	{
		prerenderSetup(level);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
}


void EndPatchRenderer::prerenderSetup(int level)
{
	assert(indexBufferGenerated);
	assert(ibo != std::numeric_limits<GLuint>::max());
	assert(vboRef == controlMesh->vbo);

	glUseProgram(program);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, controlMesh->vbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ovbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nibo);
}


void EndPatchRenderer::generateIndexBuffer()
{
	if (indexBufferGenerated)
	{
		return;
	}

	// Resize buffers
	std::shared_ptr<CCLevel> level = controlMesh->levels.back();
	int numEndPatches = level->endPatches.size();
	int numEndPatchVertices = level->vlist.size();

	numIndices = numEndPatches * 6; // 2 triangles
	endPatchIndexBuffer.resize(numIndices);

	const std::vector<Face *> &endPatches = level->endPatches;
	int firstVertexOffset = level->firstVertexOffset;
	std::vector<unsigned> &ib = endPatchIndexBuffer;

	auto updateIndexBuffer = [firstVertexOffset](const std::vector<Vertex *> &vs, unsigned *ib)
	{
		ib[0] = firstVertexOffset + vs[0]->idx;
		ib[1] = firstVertexOffset + vs[1]->idx;
		ib[2] = firstVertexOffset + vs[2]->idx;
		ib[3] = firstVertexOffset + vs[2]->idx;
		ib[4] = firstVertexOffset + vs[3]->idx;
		ib[5] = firstVertexOffset + vs[0]->idx;
	};

	// Fill the index buffer for this level
	for (int j = 0; j < endPatches.size(); ++j)
	{
		Face *f = endPatches[j];
		std::vector<Vertex *> fvs;

		f->getVertices(fvs);
		updateIndexBuffer(fvs, &ib[j * 6]);
	}

	// Create index buffer on GPU
	if (numIndices > 0)
	{
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned), ib.data(), GL_STATIC_DRAW);
	}
	ib.clear();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind

	// Fill the offsetValenceBuffer and neighbourIndexBuffer for this level
	std::vector<Vertex> &vlist = level->vlist;
	std::vector<int> &ovb = offsetValenceBuffer;
	std::vector<int> &nib = neighbourIndexBuffer;
	int accumOffset = 0;

	for (int i = 0; i < vlist.size(); ++i)
	{
		Vertex *v = &vlist[i];

		if (v->valence < 0)
		{
			// place holders
			ovb.push_back(accumOffset);
			ovb.push_back(v->valence);
		}
		else
		{
			ovb.push_back(accumOffset);
			ovb.push_back(v->valence);
			v->getOneRingIndices(firstVertexOffset, nib);
			accumOffset += 2 * v->valence;
		}
	}

	// Create correspondence on GPU
	if (numIndices > 0)
	{
		glGenBuffers(1, &ovbo);
		glGenBuffers(1, &nibo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ovbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, ovb.size() * sizeof(int), ovb.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, nibo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, nib.size() * sizeof(int), nib.data(), GL_STATIC_DRAW);
	}
	ovb.clear();
	nib.clear();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	indexBufferGenerated = true;
}


void EndPatchRenderer::createShaderProgram()
{
	if (isGLSetup)
	{
		return;
	}

	std::string vsFileName(SHADER_DIR);
	std::string fsFileName(SHADER_DIR);
	vsFileName += "/endpatch_vs.glsl";
	fsFileName += "/commonpatch_fs.glsl";
	std::vector<GLenum> types;
	std::vector<const char *> fileNames;

	types.push_back(GL_VERTEX_SHADER);
	types.push_back(GL_FRAGMENT_SHADER);
	fileNames.push_back(vsFileName.c_str());
	fileNames.push_back(fsFileName.c_str());

	ShaderHelper::createProgramWithShaders(types, fileNames, program);
	isGLSetup = true;
}


FullPatchRenderer::~FullPatchRenderer()
{
	if (indexBufferGenerated)
	{
		for (int i = 0; i < ibos.size(); ++i)
		{
			if (ibos[i] != std::numeric_limits<GLuint>::max())
			{
				glDeleteBuffers(1, &ibos[i]);
			}
		}
	}

	if (isGLSetup)
	{
		ShaderHelper::deleteProgram(program);
	}
}


void FullPatchRenderer::renderLevel(int level)
{
	//if (numIndices[level] > 0)
	//{
	//	prerenderSetup(level);
	//	glDrawElements(GL_PATCHES, numIndices[level], GL_UNSIGNED_INT, 0);
	//}
	//if( level != 2 )  return;
	if( numIndices[level] > 0 )
	{
		programs[level].draw( camera->view, camera->proj );
	}
	//camera
}


void FullPatchNoSharpRenderer::createShaderProgram()
{
	if (isGLSetup)
	{
		return;
	}

	std::string vsFileName(SHADER_DIR);
	std::string tcsFileName(SHADER_DIR);
	std::string tesFileName(SHADER_DIR);
	std::string fsFileName(SHADER_DIR);
	vsFileName += "/commonpatch_vs.glsl";
	tcsFileName += "/fullpatch_nosharp_tcs.glsl";
	tesFileName += "/fullpatch_nosharp_tes.glsl";
	fsFileName += "/commonpatch_fs.glsl";
	std::vector<GLenum> types;
	std::vector<const char *> fileNames;

	types.push_back(GL_VERTEX_SHADER);
	types.push_back(GL_TESS_CONTROL_SHADER);
	types.push_back(GL_TESS_EVALUATION_SHADER);
	types.push_back(GL_FRAGMENT_SHADER);
	fileNames.push_back(vsFileName.c_str());
	fileNames.push_back(tcsFileName.c_str());
	fileNames.push_back(tesFileName.c_str());
	fileNames.push_back(fsFileName.c_str());

	ShaderHelper::createProgramWithShaders(types, fileNames, program);
	isGLSetup = true;
}


void FullPatchNoSharpRenderer::prerenderSetup(int level)
{
	assert(indexBufferGenerated);
	assert(ibos[level] != std::numeric_limits<GLuint>::max());

	glUseProgram(program);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
}

void FullPatchNoSharpRenderer::createFullPatchPrograms( GLuint vaoHandle )
{
	if( indexBufferGenerated )
	{
		return;
	}
	// Resize buffers
	int numLevels = controlMesh->levels.size();
	numIndices.resize( numLevels, 0 );
	ibos.resize( numLevels, std::numeric_limits<GLuint>::max() );
	fullPatchIndexBuffers.resize( numLevels );
	for( int i = 0; i < numLevels; ++i )
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		int numFullPatchesNoSharp = level->fullPatchesNoSharp.size();
		numIndices[i] = numFullPatchesNoSharp * 16;
		fullPatchIndexBuffers[i].resize( numIndices[i] );
	}

	for( int i = 0; i < numLevels; ++i )
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		const std::vector<Face *> &fullPatchesNoSharp = level->fullPatchesNoSharp;
		int firstVertexOffset = level->firstVertexOffset;
		std::vector<unsigned> &ib = fullPatchIndexBuffers[i];
		int tessLevel = (int)fmax( 1.0f, baseTessFactor / pow( 2.0f, (float)i ) );

		// Fill the index buffer for this level
		for( int j = 0; j < fullPatchesNoSharp.size(); ++j )
		{
			Face *f = fullPatchesNoSharp[j];
			f->getOneRingIndices( firstVertexOffset, &ib[j * 16] );
		}

		// Create index buffer on GPU
		if( numIndices[i] > 0 )
		{
			FullPatchProgram newProgram;
			newProgram.init( vaoHandle, controlMesh->vbo );  //Create with IBO
			newProgram.use();
			//newProgram.init( controlMesh->vbo );  //Create with IBO
			newProgram.setIndices( ib.data(), (GLushort)numIndices[i] );
			newProgram.setUniform( "u_objectColor", vec3( 1.0f, 0, 0 ) );
			newProgram.setUniform( "u_tessLevelInner", tessLevel );
			newProgram.setUniform( "u_tessLevelOuter", tessLevel );
			programs.push_back( newProgram );
		}
		else
		{
			programs.push_back( FullPatchProgram() ); //TODO Temp
		}
		ib.clear();
	}
}

void FullPatchNoSharpRenderer::generateIndexBuffer()
{
	if (indexBufferGenerated)
	{
		return;
	}

	// Resize buffers
	int numLevels = controlMesh->levels.size();
	numIndices.resize(numLevels, 0);
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	fullPatchIndexBuffers.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		int numFullPatchesNoSharp = level->fullPatchesNoSharp.size();
		numIndices[i] = numFullPatchesNoSharp * 16;
		fullPatchIndexBuffers[i].resize(numIndices[i]);
	}

	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		const std::vector<Face *> &fullPatchesNoSharp = level->fullPatchesNoSharp;
		int firstVertexOffset = level->firstVertexOffset;
		std::vector<unsigned> &ib = fullPatchIndexBuffers[i];

		// Fill the index buffer for this level
		for (int j = 0; j < fullPatchesNoSharp.size(); ++j)
		{
			Face *f = fullPatchesNoSharp[j];
			f->getOneRingIndices(firstVertexOffset, &ib[j * 16]);
		}

		// Create index buffer on GPU
		if (numIndices[i] > 0)
		{
			glGenBuffers(1, &ibos[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices[i] * sizeof(unsigned), ib.data(), GL_STATIC_DRAW);
		}
		ib.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	fullPatchIndexBuffers.clear();

	indexBufferGenerated = true;
}