#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H

#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"


class ShaderHelper
{
public:
	static bool createProgramWithShaders(const std::vector<GLenum> &types, const std::vector<const char *> &fileNames, GLuint &program);

	static bool createShaderFromFile(GLenum type, const char *fileName, GLuint &shader);

	static GLint getUniformLocation(GLuint program, const char *_name, bool suppressError = false);

	static void cleanup();

private:
	static std::vector<GLuint> programs;
	static std::unordered_map<GLuint, std::unordered_map<std::string, GLint> > uniformLocations;

	ShaderHelper() {}
};

#endif // SHADER_HELPER_H