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
	virtual void renderLevel(int level) = 0;

protected:
	// Bind index buffer
	virtual void prerenderSetup(int level) = 0;
};


class FullPatchRenderer : public PatchRenderer
{
public:
	vector<FullPatchProgram> programs;
	std::vector<int> numIndices; // number of indices in each level
	std::vector<GLuint> ibos; // GL index buffers for all levels
	std::vector<std::vector<unsigned> > fullPatchIndexBuffers; // each level has one index buffer

	GLuint program;

	FullPatchRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c) : PatchRenderer(btf, cm, c) {}

	// Delete index buffer and shader program.
	virtual ~FullPatchRenderer();

	// Make sure vertex buffer is bind, input layout is specified before this
	virtual void renderLevel(int level);
};


class FullPatchNoSharpRenderer : public FullPatchRenderer
{
public:


	FullPatchNoSharpRenderer(float btf, std::shared_ptr<ControlMesh> cm, std::shared_ptr<Camera> c)
		: FullPatchRenderer(btf, cm, c)
	{
		//createShaderProgram();
	}

	virtual ~FullPatchNoSharpRenderer() {}

	virtual void generateIndexBuffer();

	virtual void createShaderProgram();

protected:
	virtual void prerenderSetup(int level);
};

#endif // PATCH_RENDERER_H