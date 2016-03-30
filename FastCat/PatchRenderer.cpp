#include "PatchRenderer.h"
#include "ShaderHelper.h"


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
		glDeleteProgram(program);
	}
}


void FullPatchRenderer::renderLevel(int level)
{
	if (numIndices[level] > 0)
	{
		prerenderSetup(level);
		glDrawElements(GL_PATCHES, numIndices[level], GL_UNSIGNED_INT, 0);
	}
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
	assert(ibos != std::numeric_limits<GLuint>::max());

	glUseProgram(program);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
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