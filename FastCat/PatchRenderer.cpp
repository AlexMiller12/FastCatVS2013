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


void partialPatchCreateProgramHelper(const char *prefix,
									 const char *tcsName, const std::vector<std::string> *tcsMacros,
									 const char *tesName, const std::vector<std::string> *tesMacros,
									 GLuint *p_program)
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

	std::vector<std::vector<std::string> > shaderMacros;
	shaderMacros.resize(4);
	shaderMacros[1] = *tcsMacros;
	shaderMacros[2] = *tesMacros;

	ShaderHelper::createProgramWithShaders(types, fileNames, *p_program, &shaderMacros);
}


void PartialPatchNoSharpCase0::createShaderProgram()
{
	programs.resize(3); // case 0 - numTriHeads == 1
	std::vector<std::string> macros(1);

	macros[0] = "CASE00";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE01";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE02";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[2]);
}


void PartialPatchNoSharpCase1::createShaderProgram()
{
	programs.resize(4); // case 1 - numTriHeads == 2 && isConnected
	std::vector<std::string> macros(1);

	macros[0] = "CASE10";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE11";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE12";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE13";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchNoSharpCase2::createShaderProgram()
{
	programs.resize(4); // case 2 - numTriHeads == 3
	std::vector<std::string> macros(1);

	macros[0] = "CASE20";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE21";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE22";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE23";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchNoSharpCase3::createShaderProgram()
{
	programs.resize(4); // case 3 - numTriHeads == 4
	std::vector<std::string> macros(1);

	macros[0] = "CASE30";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE31";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE32";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE33";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchNoSharpCase4::createShaderProgram()
{
	programs.resize(2); // case 4 - numTriHeads == 2 && !isConnected
	std::vector<std::string> macros(1);

	macros[0] = "CASE40";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE41";
	partialPatchCreateProgramHelper("/partialpatch_nosharp/",
		"partialpatch_nosharp_tcs.glsl", &macros,
		"partialpatch_nosharp_tes.glsl", &macros,
		&programs[1]);
}


void PartialPatchSharpCase0::createShaderProgram()
{
	programs.resize(3); // case 0 - numTriHeads == 1
	std::vector<std::string> macros(1);

	macros[0] = "CASE00";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE01";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE02";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[2]);
}


void PartialPatchSharpCase1::createShaderProgram()
{
	programs.resize(4); // case 1 - numTriHeads == 2 && isConnected
	std::vector<std::string> macros(1);

	macros[0] = "CASE10";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE11";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE12";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE13";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchSharpCase2::createShaderProgram()
{
	programs.resize(4); // case 2 - numTriHeads == 3
	std::vector<std::string> macros(1);

	macros[0] = "CASE20";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE21";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE22";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE23";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchSharpCase3::createShaderProgram()
{
	programs.resize(4); // case 3 - numTriHeads == 4
	std::vector<std::string> macros(1);

	macros[0] = "CASE30";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE31";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[1]);

	macros[0] = "CASE32";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[2]);

	macros[0] = "CASE33";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[3]);
}


void PartialPatchSharpCase4::createShaderProgram()
{
	programs.resize(2); // case 4 - numTriHeads == 2 && !isConnected
	std::vector<std::string> macros(1);

	macros[0] = "CASE40";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[0]);

	macros[0] = "CASE41";
	partialPatchCreateProgramHelper("/partialpatch_sharp/",
		"partialpatch_sharp_tcs.glsl", &macros,
		"partialpatch_sharp_tes.glsl", &macros,
		&programs[1]);
}


void PartialPatchNoSharpCaseX::generateIndexBuffer(const std::vector<std::vector<Face *> > &patches, std::shared_ptr<ControlMesh> cm)
{
	int numLevels = patches.size();
	numIndices.resize(numLevels, 0);
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	partialPatchIndexBuffers.resize(numLevels);
	uvBuffers.resize(numLevels, std::numeric_limits<GLuint>::max());
	uvBuffers_cpu.resize(numLevels);

	for (int i = 0; i < numLevels; ++i)
	{
		int numPatches = patches[i].size();
		numIndices[i] = numPatches * 16;
		partialPatchIndexBuffers[i].resize(numIndices[i]);
		uvBuffers_cpu[i].resize(numPatches * 8, 0.f);
	}

	for (int i = 0; i < numLevels; ++i)
	{
		int firstVertexOffset = cm->levels[i]->firstVertexOffset;
		std::vector<unsigned> &ppib = partialPatchIndexBuffers[i];
		std::vector<float> &ppuvb = uvBuffers_cpu[i];
		const std::vector<Face *> &patchesLevel = patches[i];

		for (int j = 0; j < patchesLevel.size(); ++j)
		{
			Face *f = patchesLevel[j];

			f->getOneRingIndices(firstVertexOffset, &ppib[j * 16]);

			if (f->vertexUVs.size() > 0)
			{
				std::vector<float> vertexUVs;
				f->getUVs(vertexUVs);
				memcpy(&ppuvb[j * 8], &vertexUVs[0], vertexUVs.size() * sizeof(float));
			}
		}

		if (numIndices[i] > 0)
		{
			glGenBuffers(1, &ibos[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices[i] * sizeof(unsigned), ppib.data(), GL_STATIC_DRAW);

			glGenBuffers(1, &uvBuffers[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffers[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, ppuvb.size() * sizeof(float), ppuvb.data(), GL_STATIC_DRAW);
		}
		ppib.clear();
		ppuvb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	partialPatchIndexBuffers.clear();
	uvBuffers_cpu.clear();
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

		if (uvBuffers[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &uvBuffers[i]);
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
	uvBufferOffsets.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		uvBufferOffsets[i].resize(4, 0);
	}
	ibos.resize(numLevels, std::numeric_limits<GLuint>::max());
	sbos.resize(numLevels, std::numeric_limits<GLuint>::max());
	uvBuffers.resize(numLevels, std::numeric_limits<GLuint>::max());
	partialPatchIndexBuffers.resize(numLevels);
	uvBuffers_cpu.resize(numLevels);
	sharpnessBuffers.resize(numLevels);
	for (int i = 0; i < numLevels; ++i)
	{
		int numPatches = patches[i].size();
		partialPatchIndexBuffers[i].resize(16 * numPatches);
		sharpnessBuffers[i].resize(numPatches); // each patch contain exactly one evaluable crease
		uvBuffers_cpu[i].resize(numPatches * 8, 0.f);
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
		std::vector<float> &ppuvb = uvBuffers_cpu[i];
		std::vector<int> &indexBufferOffsetSizesLevel = indexBufferOffsetSizes[i];
		std::vector<int> &sharpnessBufferOffsetsLevel = sharpnessBufferOffsets[i];
		std::vector<int> &uvBufferOffsetsLevel = uvBufferOffsets[i];

		// temporary buffers for the indices and sharpnesses in each rotation
		int numIndicesRot[4] = { 0 };
		int numPatchesRot[4] = { 0 };
		int numUVsRot[4] = { 0 };
		std::vector<std::vector<unsigned> > ibsRot;
		std::vector<std::vector<float> > uvsRot;
		std::vector<std::vector<float> > sbsRot;
		ibsRot.resize(4);
		sbsRot.resize(4);
		uvsRot.resize(4);
		for (int j = 0; j < 4; ++j)
		{
			// the sizes are upper bounds
			ibsRot[j].resize(ppib.size());
			sbsRot[j].resize(patchesLevel.size());
			uvsRot[j].resize(ppuvb.size());
		}

		for (int j = 0; j < patchesLevel.size(); ++j)
		{
			Face *f = patchesLevel[j];
			int numRotations = 0;

			while (!f->right->isSharp())
			{
				f->rotateCCW();
				++numRotations;
			}

			sbsRot[numRotations][numPatchesRot[numRotations]] = f->right->sharpness;
			++numPatchesRot[numRotations];
			f->getOneRingIndices(firstVertexOffset, &ibsRot[numRotations][numIndicesRot[numRotations]]);
			numIndicesRot[numRotations] += 16;

			if (f->vertexUVs.size() > 0)
			{
				std::vector<float> vertexUVs;
				f->getUVs(vertexUVs);
				memcpy(&uvsRot[numRotations][numUVsRot[numRotations]], &vertexUVs[0], vertexUVs.size() * sizeof(float));
				numUVsRot[numRotations] += vertexUVs.size();
			}
		}

		// pack temporary buffers into level buffers
		indexBufferOffsetSizesLevel[0] = 0;
		indexBufferOffsetSizesLevel[1] = numIndicesRot[0];
		sharpnessBufferOffsetsLevel[0] = 0;
		uvBufferOffsetsLevel[0] = 0;
		if (numIndicesRot[0] > 0)
		{
			memcpy(&ppib[0], &ibsRot[0][0], numIndicesRot[0] * sizeof(unsigned));
		}
		if (numPatchesRot[0] > 0)
		{
			memcpy(&ppsb[0], &sbsRot[0][0], numPatchesRot[0] * sizeof(float));
		}
		if (numUVsRot[0] > 0)
		{
			memcpy(&ppuvb[0], &uvsRot[0][0], numUVsRot[0] * sizeof(float));
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
			int lastUVbOffset = uvBufferOffsetsLevel[j - 1];
			int lastNumUVs = numUVsRot[j - 1];
			int curUVbOffset = lastUVbOffset + lastNumUVs;
			int curNumUVs = numUVsRot[j];

			indexBufferOffsetSizesLevel[2 * j] = curIbOffset;
			indexBufferOffsetSizesLevel[2 * j + 1] = curNumIndices;
			sharpnessBufferOffsetsLevel[j] = curSbOffset;
			uvBufferOffsetsLevel[j] = curUVbOffset;
			if (curNumIndices > 0)
			{
				memcpy(&ppib[curIbOffset], &ibsRot[j][0], curNumIndices * sizeof(unsigned));
			}
			if (curNumPatches > 0)
			{
				memcpy(&ppsb[curSbOffset], &sbsRot[j][0], curNumPatches * sizeof(float));
			}
			if (curNumUVs > 0)
			{
				memcpy(&ppuvb[curUVbOffset], &uvsRot[j][0], curNumUVs * sizeof(float));
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

			glGenBuffers(1, &uvBuffers[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffers[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, ppuvb.size() * sizeof(float), ppuvb.data(), GL_STATIC_DRAW);
		}
		ppib.clear();
		ppsb.clear();
		ppuvb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	partialPatchIndexBuffers.clear();
	sharpnessBuffers.clear();
	uvBuffers_cpu.clear();
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
		if (uvBuffers[i] != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &uvBuffers[i]);
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
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, pc->uvBuffers[level]);

			std::vector<GLuint> &programs = pc->programs;
			for (int j = 0; j < programs.size(); ++j)
			{
				glUseProgram(programs[j]);
				if (controlMesh->hasTexture)
				{
					glBindTextureUnit(4, controlMesh->diffuseMap);
				}
				if (controlMesh->hasDisplacementMap)
				{
					glBindTextureUnit(5, controlMesh->displacementMap);
				}
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
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, pc->uvBuffers[level]);

			//setDrawColor(ppDebugColorTable[i]); // Debug

			std::vector<GLuint> &programs = pc->programs;
			for (int j = 0; j < programs.size(); ++j)
			{
				glUseProgram(programs[j]);
				if (controlMesh->hasTexture)
				{
					glBindTextureUnit(4, controlMesh->diffuseMap);
				}
				if (controlMesh->hasDisplacementMap)
				{
					glBindTextureUnit(5, controlMesh->displacementMap);
				}

				for (unsigned k = 0; k < 4; ++k)
				{
					int ibOffset = pc->indexBufferOffsetSizes[level][2 * k];
					int numIndices = pc->indexBufferOffsetSizes[level][2 * k + 1];
					int sbOffset = pc->sharpnessBufferOffsets[level][k];
					int uvbOffset = pc->uvBufferOffsets[level][k];

					if (numIndices > 0)
					{
						glUniformSubroutinesuiv(GL_TESS_EVALUATION_SHADER, 1, &k);
						glUniform1i(0, sbOffset);
						glUniform1i(1, uvbOffset);
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

		if (uvBuffer != std::numeric_limits<GLuint>::max())
		{
			glDeleteBuffers(1, &uvBuffer);
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
		glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);
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
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, uvBuffer);
	if (controlMesh->hasTexture)
	{
		glBindTextureUnit(4, controlMesh->diffuseMap);
	}
	if (controlMesh->hasDisplacementMap)
	{
		glBindTextureUnit(5, controlMesh->displacementMap);
	}
	glPatchParameteri(GL_PATCH_VERTICES, 3);

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

	uvBuffer = std::numeric_limits<GLuint>::max();
	uvBuffer_cpu.resize(numIndices * 2, 0.f);

	const std::vector<Face *> &endPatches = level->endPatches;
	int firstVertexOffset = level->firstVertexOffset;
	std::vector<unsigned> &ib = endPatchIndexBuffer;
	std::vector<float> &uvb = uvBuffer_cpu;

	auto updateIndexBuffer = [firstVertexOffset](const std::vector<Vertex *> &vs, unsigned *ib)
	{
		ib[0] = firstVertexOffset + vs[0]->idx;
		ib[1] = firstVertexOffset + vs[1]->idx;
		ib[2] = firstVertexOffset + vs[2]->idx;
		ib[3] = firstVertexOffset + vs[2]->idx;
		ib[4] = firstVertexOffset + vs[3]->idx;
		ib[5] = firstVertexOffset + vs[0]->idx;
	};

	auto updateUVBuffer = [](const std::vector<float> &vertexUVs, float *uvb)
	{
		uvb[0] = vertexUVs[0]; uvb[1] = vertexUVs[1];
		uvb[2] = vertexUVs[2]; uvb[3] = vertexUVs[3];
		uvb[4] = vertexUVs[4]; uvb[5] = vertexUVs[5];
		uvb[6] = vertexUVs[4]; uvb[7] = vertexUVs[5];
		uvb[8] = vertexUVs[6]; uvb[9] = vertexUVs[7];
		uvb[10] = vertexUVs[0]; uvb[11] = vertexUVs[1];
	};

	// Fill the index buffer for this level
	for (int j = 0; j < endPatches.size(); ++j)
	{
		Face *f = endPatches[j];
		std::vector<Vertex *> fvs;

		f->getVertices(fvs);
		updateIndexBuffer(fvs, &ib[j * 6]);

		if (f->vertexUVs.size() > 0)
		{
			std::vector<float> vertexUVs;
			f->getUVs(vertexUVs);
			updateUVBuffer(vertexUVs, &uvb[j * 12]);
		}
	}

	// Create index buffer on GPU
	if (numIndices > 0)
	{
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned), ib.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, uvb.size() * sizeof(float), uvb.data(), GL_STATIC_DRAW);
	}
	ib.clear();
	uvb.clear();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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
			int startOffset = accumOffset;

			ovb.push_back(accumOffset);
			ovb.push_back(v->valence);
			v->getOneRingIndices(firstVertexOffset, nib);
			accumOffset += 2 * (-v->valence - 1) + 1;

			//if (accumOffset != nib.size())
			//{
			//	accumOffset = nib.size();
			//	nib[startOffset] = 0;
			//}

			//int debugSize = nib.size();
			//std::cout << "";
		}
		else
		{
			ovb.push_back(accumOffset);
			ovb.push_back(v->valence);
			v->getOneRingIndices(firstVertexOffset, nib);
			accumOffset += 2 * v->valence;

			//int debugSize = nib.size();
			//std::cout << "";
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
	std::string tcsFileName(SHADER_DIR);
	std::string tesFileName(SHADER_DIR);
	std::string fsFileName(SHADER_DIR);
	vsFileName += "/endpatch_vs.glsl";
	tcsFileName += "/endpatch_tcs.glsl";
	tesFileName += "/endpatch_tes.glsl";
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

			if (uvBuffers[i] != std::numeric_limits<GLuint>::max())
			{
				glDeleteBuffers(1, &uvBuffers[i]);
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
	uvBuffers_cpu.resize(numLevels);
	uvBuffers.resize(numLevels, std::numeric_limits<GLuint>::max());
	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		int numFullPatchesNoSharp = level->fullPatchesNoSharp.size();
		numIndices[i] = numFullPatchesNoSharp * 16;
		fullPatchIndexBuffers[i].resize(numIndices[i]);
		uvBuffers_cpu[i].resize(numFullPatchesNoSharp * 8, 0.f);
	}

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
		std::vector<float> &uvb = uvBuffers_cpu[i];

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

			if (f->vertexUVs.size() > 0)
			{
				std::vector<float> faceUVs;
				f->getUVs(faceUVs);
				memcpy(&uvb[j * 8], &faceUVs[0], faceUVs.size() * sizeof(float));
			}
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

			glGenBuffers(1, &uvBuffers[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffers[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, uvb.size() * sizeof(float), uvb.data(), GL_STATIC_DRAW);
		}
		ib.clear();
		sb.clear();
		uvb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	fullPatchIndexBuffers.clear();
	sharpnessBuffers.clear();
	uvBuffers_cpu.clear();

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
	if (controlMesh->hasTexture)
	{
		glBindTextureUnit(4, controlMesh->diffuseMap);
	}
	if (controlMesh->hasDisplacementMap)
	{
		glBindTextureUnit(5, controlMesh->displacementMap);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, sbos[level]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, uvBuffers[level]);
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
	if (controlMesh->hasTexture)
	{
		glBindTextureUnit(4, controlMesh->diffuseMap);
	}
	if (controlMesh->hasDisplacementMap)
	{
		glBindTextureUnit(5, controlMesh->displacementMap);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[level]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, uvBuffers[level]);
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
	uvBuffers_cpu.resize(numLevels);
	uvBuffers.resize(numLevels, std::numeric_limits<GLuint>::max());
	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		int numFullPatchesNoSharp = level->fullPatchesNoSharp.size();
		numIndices[i] = numFullPatchesNoSharp * 16;
		fullPatchIndexBuffers[i].resize(numIndices[i]);
		uvBuffers_cpu[i].resize(numFullPatchesNoSharp * 8, 0.f);
	}

	for (int i = 0; i < numLevels; ++i)
	{
		std::shared_ptr<CCLevel> level = controlMesh->levels[i];
		const std::vector<Face *> &fullPatchesNoSharp = level->fullPatchesNoSharp;
		int firstVertexOffset = level->firstVertexOffset;
		std::vector<unsigned> &ib = fullPatchIndexBuffers[i];
		std::vector<float> &uvb = uvBuffers_cpu[i];

		// Fill the index buffer for this level
		for (int j = 0; j < fullPatchesNoSharp.size(); ++j)
		{
			Face *f = fullPatchesNoSharp[j];
			f->getOneRingIndices(firstVertexOffset, &ib[j * 16]);

			if (f->vertexUVs.size() > 0)
			{
				std::vector<float> faceUVs;
				f->getUVs(faceUVs);
				memcpy(&uvb[j * 8], &faceUVs[0], faceUVs.size() * sizeof(float));
			}
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

			glGenBuffers(1, &uvBuffers[i]);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, uvBuffers[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, uvb.size() * sizeof(float), uvb.data(), GL_STATIC_DRAW);
		}
		ib.clear();
		uvb.clear();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	fullPatchIndexBuffers.clear();
	uvBuffers_cpu.clear();

	indexBufferGenerated = true;
}