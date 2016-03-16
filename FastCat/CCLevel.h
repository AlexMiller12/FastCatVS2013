#ifndef CCLEVEL_H
#define CCLEVEL_H

#include "CommonInclude.h"
#include <vector>


class CCLevel
{
public:
	CCLevel() : bufferGenerated(false) {}
	virtual ~CCLevel() {}

	void bindBuffers();

	bool bufferGenerated;
	GLuint vao, vbo;
	std::vector<float> vertexBuffer;
};

#endif // CCLEVEL_H