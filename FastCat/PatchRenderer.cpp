#include "PatchRenderer.h"
#include "ShaderHelper.h"

#include <maya/MGlobal.h>


extern GLenum g_shadingMode;


void PatchRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	baseTessFactor = btf;
	controlMesh = cm;
	indexBufferGenerated = false;
}


void EndPatchRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	PatchRenderer::changeControlMesh(btf, cm);

	numIndices = 0;
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &ovbo);
	glDeleteBuffers(1, &nibo);
	ibo = std::numeric_limits<GLuint>::max();
	ovbo = std::numeric_limits<GLuint>::max();
	nibo = std::numeric_limits<GLuint>::max();
	offsetValenceBuffer.clear();
	neighbourIndexBuffer.clear();
	endPatchIndexBuffer.clear();
}


void PartialPatchSharpCaseX::clearBuffers()
{
	indexBufferOffsetSizes.clear();
	sharpnessBufferOffsets.clear();
	partialPatchIndexBuffers.clear();
	sharpnessBuffers.clear();

	for (int i = 0; i < ibos.size(); ++i)
	{
		if (ibos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ibos[i]);
		}
	}

	for (int i = 0; i < sbos.size(); ++i)
	{
		if (sbos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &sbos[i]);
		}
	}

	ibos.clear();
	sbos.clear();
}


void PartialPatchNoSharpCaseX::clearBuffers()
{
	numIndices.clear();

	for (int i = 0; i < ibos.size(); ++i)
	{
		if (ibos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ibos[i]);
		}
	}

	ibos.clear();
	partialPatchIndexBuffers.clear();
}


void PartialPatchNoSharpRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	PatchRenderer::changeControlMesh(btf, cm);

	for (int i = 0; i < partialCases.size(); ++i)
	{
		std::shared_ptr<PartialPatchNoSharpCaseX> pc = partialCases[i];

		pc->clearBuffers();
	}

	partialPatchesCase0.clear();
	partialPatchesCase1.clear();
	partialPatchesCase2.clear();
	partialPatchesCase3.clear();
	partialPatchesCase4.clear();
}


void PartialPatchSharpRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	PatchRenderer::changeControlMesh(btf, cm);

	for (int i = 0; i < partialCases.size(); ++i)
	{
		std::shared_ptr<PartialPatchSharpCaseX> pc = partialCases[i];

		pc->clearBuffers();
	}

	partialPatchesCase0.clear();
	partialPatchesCase1.clear();
	partialPatchesCase2.clear();
	partialPatchesCase3.clear();
	partialPatchesCase4.clear();
}


void FullPatchRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	PatchRenderer::changeControlMesh(btf, cm);

	numIndices.clear();

	for (int i = 0; i < ibos.size(); ++i)
	{
		if (ibos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ibos[i]);
		}
	}

	ibos.clear();
	fullPatchIndexBuffers.clear();
}


void FullPatchSharpRenderer::changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm)
{
	FullPatchRenderer::changeControlMesh(btf, cm);

	for (int i = 0; i < sbos.size(); ++i)
	{
		if (sbos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &sbos[i]);
		}
	}

	sbos.clear();
	sharpnessBuffers.clear();
}


void partialPatchCreateProgramHelper(const char *prefix, const char *tcsName, const char *tesName, GLuint *p_program)
{
	std::string vsFileName(SHADER_DIR);
	std::string tcsFileName(SHADER_DIR);
	std::string tesFileName(SHADER_DIR);
	std::string fsFileName(SHADER_DIR);
	vsFileName += "/commonpatch_vs.glsl";
	tcsFileName += prefix;
	tcsFileName += tcsName;
	tesFileName += prefix;
	tesFileName += tesName;
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

	ShaderHelper::createProgramWithShaders(types, fileNames, *p_program);
}


void PartialPatchNoSharpCase0::createShaderProgram()
{
	programs.resize(3); // case 0 - numTriHeads == 1

	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case00_tcs.glsl",
		"partialpatch_nosharp_case00_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case01_tcs.glsl",
		"partialpatch_nosharp_case01_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case02_tcs.glsl",
		"partialpatch_nosharp_case02_tes.glsl", &programs[2]);
}


void PartialPatchNoSharpCase1::createShaderProgram()
{
	programs.resize(4); // case 1 - numTriHeads == 2 && isConnected

	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case10_tcs.glsl",
		"partialpatch_nosharp_case10_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case11_tcs.glsl",
		"partialpatch_nosharp_case11_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case12_tcs.glsl",
		"partialpatch_nosharp_case12_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case13_tcs.glsl",
		"partialpatch_nosharp_case13_tes.glsl", &programs[3]);
}


void PartialPatchNoSharpCase2::createShaderProgram()
{
	programs.resize(4); // case 2 - numTriHeads == 3

	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case20_tcs.glsl",
		"partialpatch_nosharp_case20_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case21_tcs.glsl",
		"partialpatch_nosharp_case21_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case22_tcs.glsl",
		"partialpatch_nosharp_case22_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case23_tcs.glsl",
		"partialpatch_nosharp_case23_tes.glsl", &programs[3]);
}


void PartialPatchNoSharpCase3::createShaderProgram()
{
	programs.resize(4); // case 3 - numTriHeads == 4

	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case30_tcs.glsl",
		"partialpatch_nosharp_case30_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case30_tcs.glsl",
		"partialpatch_nosharp_case31_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case30_tcs.glsl",
		"partialpatch_nosharp_case32_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case30_tcs.glsl",
		"partialpatch_nosharp_case33_tes.glsl", &programs[3]);
}


void PartialPatchNoSharpCase4::createShaderProgram()
{
	programs.resize(2); // case 4 - numTriHeads == 2 && !isConnected

	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case40_tcs.glsl",
		"partialpatch_nosharp_case40_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_nosharp/", "partialpatch_nosharp_case40_tcs.glsl",
		"partialpatch_nosharp_case41_tes.glsl", &programs[1]);
}


void PartialPatchSharpCase0::createShaderProgram()
{
	programs.resize(3); // case 0 - numTriHeads == 1

	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case00_tcs.glsl",
		"partialpatch_sharp_case00_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case01_tcs.glsl",
		"partialpatch_sharp_case01_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case02_tcs.glsl",
		"partialpatch_sharp_case02_tes.glsl", &programs[2]);
}


void PartialPatchSharpCase1::createShaderProgram()
{
	programs.resize(4); // case 1 - numTriHeads == 2 && isConnected

	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case10_tcs.glsl",
		"partialpatch_sharp_case10_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case11_tcs.glsl",
		"partialpatch_sharp_case11_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case12_tcs.glsl",
		"partialpatch_sharp_case12_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case13_tcs.glsl",
		"partialpatch_sharp_case13_tes.glsl", &programs[3]);
}


void PartialPatchSharpCase2::createShaderProgram()
{
	programs.resize(4); // case 2 - numTriHeads == 3

	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case20_tcs.glsl",
		"partialpatch_sharp_case20_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case21_tcs.glsl",
		"partialpatch_sharp_case21_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case22_tcs.glsl",
		"partialpatch_sharp_case22_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case23_tcs.glsl",
		"partialpatch_sharp_case23_tes.glsl", &programs[3]);
}


void PartialPatchSharpCase3::createShaderProgram()
{
	programs.resize(4); // case 3 - numTriHeads == 4

	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case30_tcs.glsl",
		"partialpatch_sharp_case30_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case30_tcs.glsl",
		"partialpatch_sharp_case31_tes.glsl", &programs[1]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case30_tcs.glsl",
		"partialpatch_sharp_case32_tes.glsl", &programs[2]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case30_tcs.glsl",
		"partialpatch_sharp_case33_tes.glsl", &programs[3]);
}


void PartialPatchSharpCase4::createShaderProgram()
{
	programs.resize(2); // case 4 - numTriHeads == 2 && !isConnected

	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case40_tcs.glsl",
		"partialpatch_sharp_case40_tes.glsl", &programs[0]);
	partialPatchCreateProgramHelper("/partialpatch_sharp/", "partialpatch_sharp_case40_tcs.glsl",
		"partialpatch_sharp_case41_tes.glsl", &programs[1]);
}


void PartialPatchNoSharpCaseX::generateIndexBuffer(const std::vector<std::vector<Face *> > &patches, std::shared_ptr<ControlMesh> cm)
{
	int numLevels = patches.size();
	numIndices.resize(numLevels, 0);
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	partialPatchIndexBuffers.resize(numLevels);

	for (int i = 0; i < numLevels; ++i)
	{
		int numPatches = patches[i].size();
		numIndices[i] = numPatches * 16;
		partialPatchIndexBuffers[i].resize(numIndices[i]);
	}

	for (int i = 0; i < numLevels; ++i)
	{
		int firstVertexOffset = cm->levels[i]->firstVertexOffset;
		std::vector<unsigned> &ppib = partialPatchIndexBuffers[i];
		const std::vector<Face *> &patchesLevel = patches[i];

		for (int j = 0; j < patchesLevel.size(); ++j)
		{
			Face *f = patchesLevel[j];

			f->getOneRingIndices(firstVertexOffset, &ppib[j * 16]);
		}

		if (numIndices[i] > 0)
		{
			glGenBuffers(1, &ibos[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices[i] * sizeof(unsigned), ppib.data(), GL_STATIC_DRAW);
		}
		ppib.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	partialPatchIndexBuffers.clear();
}


PartialPatchNoSharpCaseX::~PartialPatchNoSharpCaseX()
{
	int numLevels = numIndices.size();

	for (int i = 0; i < numLevels; ++i)
	{
		if (numIndices[i] > 0)
		{
			glDeleteBuffers(1, &ibos[i]);
		}
	}

	for (int i = 0; i < programs.size(); ++i)
	{
		ShaderHelper::deleteProgram(programs[i]);
	}
}


void PartialPatchSharpCaseX::generateIndexBuffer(const std::vector<std::vector<Face *> > &patches,
	std::shared_ptr<ControlMesh> cm)
{
	// resize buffers
	int numLevels = patches.size();
	indexBufferOffsetSizes.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		indexBufferOffsetSizes[i].resize(8, 0); // 4 rotations. 1 offset and 1 numIndices for each rotation
	}
	sharpnessBufferOffsets.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		sharpnessBufferOffsets[i].resize(4, 0); // 1 offset for each rotation
	}
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	sbos.resize(numLevels, std::numeric_limits<GLuint>::max());
	partialPatchIndexBuffers.resize(numLevels);
	sharpnessBuffers.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		int numPatches = patches[i].size();
		partialPatchIndexBuffers[i].resize(16 * numPatches);
		sharpnessBuffers[i].resize(numPatches); // each patch contain exactly one evaluable crease
	}

	// build CPU buffers
	for (int i = 0; i < numLevels; ++i)
	{
		const std::vector<Face *> &patchesLevel = patches[i];

		if (patchesLevel.size() == 0)
		{
			continue;
		}

		int firstVertexOffset = cm->levels[i]->firstVertexOffset;
		std::vector<unsigned> &ppib = partialPatchIndexBuffers[i];
		std::vector<float> &ppsb = sharpnessBuffers[i];
		std::vector<int> &indexBufferOffsetSizesLevel = indexBufferOffsetSizes[i];
		std::vector<int> &sharpnessBufferOffsetsLevel = sharpnessBufferOffsets[i];

		// temporary buffers for the indices and sharpnesses in each rotation
		int numIndicesRot[4] = { 0 };
		int numPatchesRot[4] = { 0 };
		std::vector<std::vector<unsigned> > ibsRot;
		std::vector<std::vector<float> > sbsRot;
		ibsRot.resize(4);
		sbsRot.resize(4);
		for (int j = 0; j < 4; ++j)
		{
			// the sizes are upper bounds
			ibsRot[j].resize(ppib.size());
			sbsRot[j].resize(patchesLevel.size());
		}

		for (int j = 0; j < patchesLevel.size(); ++j)
		{
			Face *f = patchesLevel[j];
			int numRotatins = 0;

			while (!f->right->isSharp())
			{
				f->rotateCCW();
				++numRotatins;
			}

			sbsRot[numRotatins][numPatchesRot[numRotatins]] = f->right->sharpness;
			++numPatchesRot[numRotatins];
			f->getOneRingIndices(firstVertexOffset, &ibsRot[numRotatins][numIndicesRot[numRotatins]]);
			numIndicesRot[numRotatins] += 16;
		}

		// pack temporary buffers into level buffers
		indexBufferOffsetSizesLevel[0] = 0;
		indexBufferOffsetSizesLevel[1] = numIndicesRot[0];
		sharpnessBufferOffsetsLevel[0] = 0;
		if (numIndicesRot[0] > 0)
		{
			memcpy(&ppib[0], &ibsRot[0][0], numIndicesRot[0] * sizeof(unsigned));
		}
		if (numPatchesRot[0] > 0)
		{
			memcpy(&ppsb[0], &sbsRot[0][0], numPatchesRot[0] * sizeof(float));
		}

		for (int j = 1; j < 4; ++j)
		{
			int lastIbOffset = indexBufferOffsetSizesLevel[2 * (j - 1)];
			int lastNumIndices = indexBufferOffsetSizesLevel[2 * (j - 1) + 1];
			int curIbOffset = lastIbOffset + lastNumIndices;
			int curNumIndices = numIndicesRot[j];
			int lastSbOffset = sharpnessBufferOffsetsLevel[j - 1];
			int lastNumPatches = numPatchesRot[j - 1];
			int curSbOffset = lastSbOffset + lastNumPatches;
			int curNumPatches = numPatchesRot[j];

			indexBufferOffsetSizesLevel[2 * j] = curIbOffset;
			indexBufferOffsetSizesLevel[2 * j + 1] = curNumIndices;
			sharpnessBufferOffsetsLevel[j] = curSbOffset;
			if (curNumIndices > 0)
			{
				memcpy(&ppib[curIbOffset], &ibsRot[j][0], curNumIndices * sizeof(unsigned));
			}
			if (curNumPatches > 0)
			{
				memcpy(&ppsb[curSbOffset], &sbsRot[j][0], curNumPatches * sizeof(float));
			}
		}

		// Generate GPU buffers
		if (patchesLevel.size() > 0)
		{
			glGenBuffers(1, &ibos[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ppib.size() * sizeof(unsigned), ppib.data(), GL_STATIC_DRAW);

			glGenBuffers(1, &sbos[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbos[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, ppsb.size() * sizeof(float), ppsb.data(), GL_STATIC_DRAW);
		}
		ppib.clear();
		ppsb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	partialPatchIndexBuffers.clear();
	sharpnessBuffers.clear();
}


PartialPatchSharpCaseX::~PartialPatchSharpCaseX()
{
	int numLevels = indexBufferOffsetSizes.size();

	for (int i = 0; i < numLevels; ++i)
	{
		if (ibos[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &ibos[i]);
			glDeleteBuffers(1, &sbos[i]);
		}
	}

	for (int i = 0; i < programs.size(); ++i)
	{
		ShaderHelper::deleteProgram(programs[i]);
	}
}


// Debug
static const glm::vec4 ppDebugColorTable[5] =
{
	glm::vec4(1.f, 1.f, .5f, 1.f),
	glm::vec4(0.f, 1.f, 0.f, 1.f),
	glm::vec4(1.f, 0.f, 0.f, 1.f),
	glm::vec4(0.f, 0.f, 1.f, 1.f),
	glm::vec4(1.f, 0.f, 1.f, 1.f)
};


void PartialPatchNoSharpRenderer::renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor)
{
	prerenderSetup(level, setDrawColor);

	for (int i = 0; i < partialCases.size(); ++i)
	{
		//setDrawColor(ppDebugColorTable[i]); // Debug

		std::shared_ptr<PartialPatchNoSharpCaseX> pc = partialCases[i];

		if (pc->numIndices[level] > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pc->ibos[level]);

			std::vector<GLuint> &programs = pc->programs;
			for (int j = 0; j < programs.size(); ++j)
			{
				glUseProgram(programs[j]);
				glDrawElements(GL_PATCHES, pc->numIndices[level], GL_UNSIGNED_INT, 0);
			}
		}
	}
}


void PartialPatchNoSharpRenderer::prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor)
{
	setDrawColor(glm::vec4(1.f, 1.f, 0.f, 1.f)); // Debug

	glPolygonMode(GL_FRONT_AND_BACK, g_shadingMode);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
}


void PartialPatchNoSharpRenderer::createShaderProgram()
{
	if (isGLSetup)
	{
		return;
	}

	for (int i = 0; i < partialCases.size(); ++i)
	{
		partialCases[i]->createShaderProgram();
	}

	isGLSetup = true;
}


void PartialPatchNoSharpRenderer::generateIndexBuffer()
{
	if (indexBufferGenerated)
	{
		return;
	}

	orderPartialPatchesToCases(controlMesh, false); // non-sharp

	partialCases[0]->generateIndexBuffer(partialPatchesCase0, controlMesh);
	partialCases[1]->generateIndexBuffer(partialPatchesCase1, controlMesh);
	partialCases[2]->generateIndexBuffer(partialPatchesCase2, controlMesh);
	partialCases[3]->generateIndexBuffer(partialPatchesCase3, controlMesh);
	partialCases[4]->generateIndexBuffer(partialPatchesCase4, controlMesh);

	indexBufferGenerated = true;
}


void PartialPatchSharpRenderer::prerenderSetup(int level, std::function<void(const glm::vec4 &)> setDrawColor)
{
	setDrawColor(glm::vec4(1.f, 0.f, 0.f, 1.f)); // Debug

	glPolygonMode(GL_FRONT_AND_BACK, g_shadingMode);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
}


void PartialPatchSharpRenderer::renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor)
{
	prerenderSetup(level, setDrawColor);

	for (int i = 0; i < partialCases.size(); ++i)
	{
		std::shared_ptr<PartialPatchSharpCaseX> pc = partialCases[i];

		if (pc->ibos[level] != std::numeric_limits<GLuint>::max())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pc->ibos[level]);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, pc->sbos[level]);

			//setDrawColor(ppDebugColorTable[i]); // Debug

			std::vector<GLuint> &programs = pc->programs;
			for (int j = 0; j < programs.size(); ++j)
			{
				glUseProgram(programs[j]);

				for (unsigned k = 0; k < 4; ++k)
				{
					int ibOffset = pc->indexBufferOffsetSizes[level][2 * k];
					int numIndices = pc->indexBufferOffsetSizes[level][2 * k + 1];
					int sbOffset = pc->sharpnessBufferOffsets[level][k];

					if (numIndices > 0)
					{
						glUniformSubroutinesuiv(GL_TESS_EVALUATION_SHADER, 1, &k);
						glUniform1i(0, sbOffset);
						glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, (const void *)(ibOffset * sizeof(unsigned)));
					}
				}
			}
		}
	}
}


void PartialPatchSharpRenderer::createShaderProgram()
{
	if (isGLSetup)
	{
		return;
	}

	for (int i = 0; i < partialCases.size(); ++i)
	{
		partialCases[i]->createShaderProgram();
	}

	isGLSetup = true;
}


void PartialPatchSharpRenderer::generateIndexBuffer()
{
	if (indexBufferGenerated)
	{
		return;
	}

	orderPartialPatchesToCases(controlMesh, true); // sharp

	partialCases[0]->generateIndexBuffer(partialPatchesCase0, controlMesh);
	partialCases[1]->generateIndexBuffer(partialPatchesCase1, controlMesh);
	partialCases[2]->generateIndexBuffer(partialPatchesCase2, controlMesh);
	partialCases[3]->generateIndexBuffer(partialPatchesCase3, controlMesh);
	partialCases[4]->generateIndexBuffer(partialPatchesCase4, controlMesh);

	indexBufferGenerated = true;
}


void PartialPatchCommon::orderPartialPatchesToCases(std::shared_ptr<ControlMesh> controlMesh, bool isSharp)
{
	partialPatchesCase0.clear();
	partialPatchesCase1.clear();
	partialPatchesCase2.clear();
	partialPatchesCase3.clear();
	partialPatchesCase4.clear();

	int numLevels = controlMesh->levels.size();
	partialPatchesCase0.resize(numLevels);
	partialPatchesCase1.resize(numLevels);
	partialPatchesCase2.resize(numLevels);
	partialPatchesCase3.resize(numLevels);
	partialPatchesCase4.resize(numLevels);

	for (int i = 0; i < numLevels; ++i)
	{
		std::vector<Face *> &ppc0 = partialPatchesCase0[i];
		std::vector<Face *> &ppc1 = partialPatchesCase1[i];
		std::vector<Face *> &ppc2 = partialPatchesCase2[i];
		std::vector<Face *> &ppc3 = partialPatchesCase3[i];
		std::vector<Face *> &ppc4 = partialPatchesCase4[i];
		std::vector<Face *> &partialPatches =
			isSharp? controlMesh->levels[i]->partialPatchesSharp : controlMesh->levels[i]->partialPatchesNoSharp;

		for (int j = 0; j < partialPatches.size(); ++j)
		{
			int numTriangleHeads;
			bool isConnected;
			Face *f = partialPatches[j];

			f->getPartialPatchInfo(&numTriangleHeads, &isConnected);

			if (numTriangleHeads == 1) // case 0
			{
				while (!f->right->isTriangleHead)
				{
					f->rotateCCW();
				}
				ppc0.push_back(f);
			}
			else if (numTriangleHeads == 2 && isConnected) // case 1
			{
				while (!f->right->fPrev()->isTriangleHead || !f->right->isTriangleHead)
				{
					f->rotateCCW();
				}
				ppc1.push_back(f);
			}
			else if (numTriangleHeads == 3) // case 2
			{
				while (f->right->isTriangleHead)
				{
					f->rotateCCW();
				}
				ppc2.push_back(f);
			}
			else if (numTriangleHeads == 4) // case 3
			{
				ppc3.push_back(f);
			}
			else if (numTriangleHeads == 2 && !isConnected) // case 4
			{
				while (!f->right->isTriangleHead)
				{
					f->rotateCCW();
				}
				ppc4.push_back(f);
			}
			else
			{
				MGlobal::displayError("Unknown partial patch type");
				return;
			}
		}
	}
}


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


void EndPatchRenderer::renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor)
{
	// Only render end patches in the last subdivision level
	if (level == controlMesh->maxSubdivisionLevel && numIndices > 0)
	{
		prerenderSetup(level, setDrawColor);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	}
}


void EndPatchRenderer::prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor)
{
	assert(indexBufferGenerated);
	assert(ibo != std::numeric_limits<GLuint>::max());
	assert(vboRef == controlMesh->vbo);

	setDrawColor(glm::vec4(0.f, 1.f, 1.f, 1.f)); // Debug

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


void FullPatchRenderer::renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor)
{
	if (numIndices[level] > 0)
	{
		prerenderSetup(level, setDrawColor);
		glDrawElements(GL_PATCHES, numIndices[level], GL_UNSIGNED_INT, 0);
	}
}


FullPatchSharpRenderer::~FullPatchSharpRenderer()
{
	if (indexBufferGenerated)
	{
		for (int i = 0; i < sbos.size(); ++i)
		{
			if (sbos[i] != std::numeric_limits<GLuint>::max())
			{
				glDeleteBuffers(1, &sbos[i]);
			}
		}
	}
}


void FullPatchSharpRenderer::generateIndexBuffer()
{
	if (indexBufferGenerated)
	{
		return;
	}

	// Resize buffers
	int numLevels = controlMesh->levels.size();
	numIndices.resize(numLevels, 0);
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	sbos.resize(numLevels, std::numeric_limits<GLuint>::max());
	fullPatchIndexBuffers.resize(numLevels);
	sharpnessBuffers.resize(numLevels);

	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		int numFullPatchesSharp = level->fullPatchesSharp.size();
		numIndices[i] = numFullPatchesSharp * 16;
		fullPatchIndexBuffers[i].resize(numIndices[i]);
		sharpnessBuffers[i].resize(numFullPatchesSharp); // each patch has exactly one evaluable sharp edge
	}

	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		const std::vector<Face *> &fullPatchesSharp = level->fullPatchesSharp;
		int firstVertexOffset = level->firstVertexOffset;
		std::vector<unsigned> &ib = fullPatchIndexBuffers[i];
		std::vector<float> &sb = sharpnessBuffers[i];
		int numSharpFullPatches = fullPatchesSharp.size();

		// Fill the index buffer for this level
		for (int j = 0; j < numSharpFullPatches; ++j)
		{
			Face *f = fullPatchesSharp[j];

			while (!f->right->isSharp())
			{
				f->rotateCCW();
			}

			sb[j] = f->right->sharpness;
			f->getOneRingIndices(firstVertexOffset, &ib[j * 16]);
		}

		// Create index buffer on GPU
		if (numIndices[i] > 0)
		{
			glGenBuffers(1, &ibos[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices[i] * sizeof(unsigned), ib.data(), GL_STATIC_DRAW);

			glGenBuffers(1, &sbos[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, sbos[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, numSharpFullPatches * sizeof(float), sb.data(), GL_STATIC_DRAW);
		}
		ib.clear();
		sb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	fullPatchIndexBuffers.clear();
	sharpnessBuffers.clear();

	indexBufferGenerated = true;
}


void FullPatchSharpRenderer::createShaderProgram()
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
	tcsFileName += "/fullpatch_sharp_tcs.glsl";
	tesFileName += "/fullpatch_sharp_tes.glsl";
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


void FullPatchSharpRenderer::prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor)
{
	setDrawColor(glm::vec4(1.f, .5f, 1.f, 1.f)); // Debug

	glUseProgram(program);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, sbos[level]);
	glPolygonMode(GL_FRONT_AND_BACK, g_shadingMode);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
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


void FullPatchNoSharpRenderer::prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor)
{
	assert(indexBufferGenerated);
	assert(ibos[level] != std::numeric_limits<GLuint>::max());

	setDrawColor(glm::vec4(1.f, 1.f, 1.f, 1.f)); // Debug

	glUseProgram(program);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glPolygonMode(GL_FRONT_AND_BACK, g_shadingMode);
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
			//TODO Temp:
			//FullPatchProgram newProgram;
			//newProgram.init( controlMesh->vbo );  //Create with IBO
			//newProgram.use();
			////newProgram.init( controlMesh->vbo );  //Create with IBO
			//newProgram.setIndices( ib.data(), (GLushort)numIndices[i] );
			//newProgram.setUniform( "u_objectColor", vec3( 1.0f, 0, 0 ) );

			//int tessLevel = (int)fmax( 1.0f, baseTessFactor / pow( 2.0f, (float)i ) );
			//
			//newProgram.setUniform( "u_tessLevelInner", tessLevel );
			//newProgram.setUniform( "u_tessLevelOuter", tessLevel );
			//programs.push_back( newProgram );
			//-----
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