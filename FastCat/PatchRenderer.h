#ifndef PATCH_RENDERER_H
#define PATCH_RENDERER_H

#include "ControlMesh.h"
#include "Camera.h"
#include "FullPatchProgram.h"


// Base class for renderer of all types of patches
class PatchRenderer
{
public:
	float baseTessFactor;
	std::shared_ptr<ControlMesh> controlMesh;
	std::shared_ptr<Camera> camera;

	bool indexBufferGenerated;
	bool isGLSetup;

	// @btf - tessellation factor for the base subdivision level (no subdivision)
	PatchRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: baseTessFactor(btf), controlMesh(cm), camera(c),
		indexBufferGenerated(false), isGLSetup(false) {}

	virtual ~PatchRenderer() {}

	// Different patches may need different shaders to render them
	virtual void createShaderProgram() = 0;

	// Generate index buffer for all the patches(faces) in one of the 5
	// patch arrays in each subdivision level.
	// Only call this method after calling @controlMesh->adaptiveCCAllLevels()
	virtual void generateIndexBuffer() = 0;

	// Render one type of patches for all subdivision levels
	// Also load in shaders, create shader program, generate and bind index buffer
	// if these haven't been done
	virtual void renderLevel(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr) = 0;

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);

protected:
	// Bind index buffer
	virtual void prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr) = 0;
};


// Only for last subdvision level
class EndPatchRenderer : public PatchRenderer
{
public:
	int numIndices;
	GLuint ibo;
	GLuint ovbo; // handle to offsetValenceBuffers on GPU
	GLuint nibo; // handle to neighbourIndexBuffers on GPU
	GLuint uvBuffer;
	std::vector<int> offsetValenceBuffer; // offsets into neighbourIdxBuffers
	std::vector<int> neighbourIndexBuffer;
	std::vector<unsigned> endPatchIndexBuffer;
	std::vector<float> uvBuffer_cpu;

	GLuint program;

	EndPatchRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: PatchRenderer(btf, cm, c) {}

	// // Delete buffers and shader program.
	virtual ~EndPatchRenderer();

	// Make sure vertex buffer is bind, input layout is specified before this
	virtual void renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor = nullptr);

	virtual void generateIndexBuffer();

	virtual void createShaderProgram();

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);

protected:
	virtual void prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr);
};


class PartialPatchCommon
{
protected:
	std::vector<std::vector<Face *> > partialPatchesCase0;
	std::vector<std::vector<Face *> > partialPatchesCase1;
	std::vector<std::vector<Face *> > partialPatchesCase2;
	std::vector<std::vector<Face *> > partialPatchesCase3;
	std::vector<std::vector<Face *> > partialPatchesCase4;

	// Also rotate them to consistent orientation
	virtual void orderPartialPatchesToCases(std::shared_ptr<ControlMesh> controlMesh, bool isSharp);
};


// Helper class for PartialPatchSharpRenderer
class PartialPatchSharpCaseX
{
public:
	std::vector<std::vector<int> > indexBufferOffsetSizes;
	std::vector<std::vector<int> > sharpnessBufferOffsets;
	std::vector<std::vector<int> > uvBufferOffsets;
	std::vector<GLuint> ibos;
	std::vector<GLuint> sbos;
	std::vector<GLuint> uvBuffers;
	std::vector<std::vector<unsigned> > partialPatchIndexBuffers;
	std::vector<std::vector<float> > sharpnessBuffers;
	std::vector<std::vector<float> > uvBuffers_cpu;

	std::vector<GLuint> programs; // one program for each subdomain

	PartialPatchSharpCaseX() {}

	// Delete index buffer, sharpness buffer, and shader program.
	virtual ~PartialPatchSharpCaseX();

	virtual void generateIndexBuffer(const std::vector<std::vector<Face *> > &patches,
		std::shared_ptr<ControlMesh> cm);

	// Different cases use different hull and domain shaders
	virtual void createShaderProgram() = 0;

	virtual void clearBuffers();
};


class PartialPatchSharpCase0 : public PartialPatchSharpCaseX
{
public:
	PartialPatchSharpCase0() {}

	virtual ~PartialPatchSharpCase0() {}

	virtual void createShaderProgram() override;
};


class PartialPatchSharpCase1 : public PartialPatchSharpCaseX
{
public:
	PartialPatchSharpCase1() {}

	virtual ~PartialPatchSharpCase1() {}

	virtual void createShaderProgram() override;
};


class PartialPatchSharpCase2 : public PartialPatchSharpCaseX
{
public:
	PartialPatchSharpCase2() {}

	virtual ~PartialPatchSharpCase2() {}

	virtual void createShaderProgram() override;
};


class PartialPatchSharpCase3 : public PartialPatchSharpCaseX
{
public:
	PartialPatchSharpCase3() {}

	virtual ~PartialPatchSharpCase3() {}

	virtual void createShaderProgram() override;
};


class PartialPatchSharpCase4 : public PartialPatchSharpCaseX
{
public:
	PartialPatchSharpCase4() {}

	virtual ~PartialPatchSharpCase4() {}

	virtual void createShaderProgram() override;
};


class PartialPatchSharpRenderer : public PatchRenderer, public PartialPatchCommon
{
public:
	std::vector<std::shared_ptr<PartialPatchSharpCaseX> > partialCases; // 5 cases in total

	PartialPatchSharpRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: PatchRenderer(btf, cm, c)
	{
		partialCases.resize(5);
		partialCases[0] = std::make_shared<PartialPatchSharpCase0>();
		partialCases[1] = std::make_shared<PartialPatchSharpCase1>();
		partialCases[2] = std::make_shared<PartialPatchSharpCase2>();
		partialCases[3] = std::make_shared<PartialPatchSharpCase3>();
		partialCases[4] = std::make_shared<PartialPatchSharpCase4>();
	}

	virtual ~PartialPatchSharpRenderer() {}

	// order partial patches according to cases and then
	// ask case renderers to genereate index buffers
	virtual void generateIndexBuffer();

	// ask the 5 case renderers to load in the shaders they need
	virtual void createShaderProgram();

	// Make sure vertex buffer is bind, input layout is specified before this
	virtual void renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor = nullptr);

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);

protected:
	virtual void prerenderSetup(int level, std::function<void(const glm::vec4 &)> setDrawColor = nullptr);
};


// Helper class for PartialPatchNoSharpRenderer
class PartialPatchNoSharpCaseX
{
public:
	std::vector<int> numIndices;
	std::vector<GLuint> ibos;
	std::vector<GLuint> uvBuffers;
	std::vector<std::vector<unsigned> > partialPatchIndexBuffers;
	std::vector<std::vector<float> > uvBuffers_cpu;

	std::vector<GLuint> programs; // one program for each subdomain
	
	PartialPatchNoSharpCaseX() {}

	// Delete index buffer and shader program.
	virtual ~PartialPatchNoSharpCaseX();

	virtual void generateIndexBuffer(const std::vector<std::vector<Face *> > &patches,
		std::shared_ptr<ControlMesh> cm);

	// Different cases use different hull and domain shaders
	virtual void createShaderProgram() = 0;

	virtual void clearBuffers();
};


class PartialPatchNoSharpCase0 : public PartialPatchNoSharpCaseX
{
public:
	PartialPatchNoSharpCase0() {}

	virtual ~PartialPatchNoSharpCase0() {}

	virtual void createShaderProgram();
};


class PartialPatchNoSharpCase1 : public PartialPatchNoSharpCaseX
{
public:
	PartialPatchNoSharpCase1() {}

	virtual ~PartialPatchNoSharpCase1() {}

	virtual void createShaderProgram();
};


class PartialPatchNoSharpCase2 : public PartialPatchNoSharpCaseX
{
public:
	PartialPatchNoSharpCase2() {}

	virtual ~PartialPatchNoSharpCase2() {}

	virtual void createShaderProgram();
};


class PartialPatchNoSharpCase3 : public PartialPatchNoSharpCaseX
{
public:
	PartialPatchNoSharpCase3() {}

	virtual ~PartialPatchNoSharpCase3() {}

	virtual void createShaderProgram();
};


class PartialPatchNoSharpCase4 : public PartialPatchNoSharpCaseX
{
public:
	PartialPatchNoSharpCase4() {}

	virtual ~PartialPatchNoSharpCase4() {}

	virtual void createShaderProgram();
};


class PartialPatchNoSharpRenderer : public PatchRenderer, public PartialPatchCommon
{
public:
	std::vector<std::shared_ptr<PartialPatchNoSharpCaseX> > partialCases; // 5 cases in total

	PartialPatchNoSharpRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: PatchRenderer(btf, cm, c)
	{
		partialCases.resize(5);
		partialCases[0] = std::make_shared<PartialPatchNoSharpCase0>();
		partialCases[1] = std::make_shared<PartialPatchNoSharpCase1>();
		partialCases[2] = std::make_shared<PartialPatchNoSharpCase2>();
		partialCases[3] = std::make_shared<PartialPatchNoSharpCase3>();
		partialCases[4] = std::make_shared<PartialPatchNoSharpCase4>();
	}

	virtual ~PartialPatchNoSharpRenderer() {}

	// order partial patches according to cases and then
	// ask case renderers to genereate index buffers
	virtual void generateIndexBuffer();

	// ask the 5 case renderers to load in the shaders they need
	virtual void createShaderProgram();

	// Make sure vertex buffer is bind, input layout is specified before this
	virtual void renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor = nullptr);

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);

protected:
	virtual void prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr);
};


class FullPatchRenderer : public PatchRenderer
{
public:
	//vector<FullPatchProgram> programs;

	std::vector<int> numIndices; // number of indices in each level
	std::vector<GLuint> ibos; // GL index buffers for all levels
	std::vector<std::vector<unsigned> > fullPatchIndexBuffers; // each level has one index buffer
	std::vector<GLuint> uvBuffers;
	std::vector<std::vector<float> > uvBuffers_cpu;

	GLuint program;

	FullPatchRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c) : PatchRenderer(btf, cm, c) {}

	// Delete index buffer and shader program.
	virtual ~FullPatchRenderer();

	// Make sure vertex buffer is bind, input layout is specified before this
	virtual void renderLevel(int level, std::function<void(const glm::vec4 &)> setDrawColor = nullptr);

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);
};


class FullPatchSharpRenderer : public FullPatchRenderer
{
public:
	std::vector<GLuint> sbos; // GPU sharpness buffers for each level
	std::vector<std::vector<float> > sharpnessBuffers;

	FullPatchSharpRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: FullPatchRenderer(btf, cm, c) {}

	virtual ~FullPatchSharpRenderer();

	virtual void generateIndexBuffer();

	virtual void createShaderProgram();

	virtual void changeControlMesh(float btf, std::shared_ptr<ControlMesh> cm);

protected:
	virtual void prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr);
};


class FullPatchNoSharpRenderer : public FullPatchRenderer
{
public:
	FullPatchNoSharpRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: FullPatchRenderer(btf, cm, c) {}

	virtual ~FullPatchNoSharpRenderer() {}

	virtual void generateIndexBuffer();

	virtual void createShaderProgram();

protected:
	virtual void prerenderSetup(int level, std::function<void (const glm::vec4 &)> setDrawColor = nullptr);
};

#endif // PATCH_RENDERER_H