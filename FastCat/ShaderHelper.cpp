#include "ShaderHelper.h"
#include "CommonInclude.h"
#include "Dependencies/maya/maya/MGlobal.h";
#include <iostream>


std::vector<GLuint> ShaderHelper::programs;
std::unordered_map<GLuint, std::unordered_map<std::string, GLint> > ShaderHelper::uniformLocations;


bool ShaderHelper::createProgramWithShaders(const std::vector<GLenum> &types, const std::vector<const char *> &fileNames, GLuint &program)
{
	if (types.size() != fileNames.size() || types.empty())
	{
		return false; // failed;
	}

	int numShaders = types.size();
	program = glCreateProgram();
	std::vector<GLuint> shaders;

	for (int i = 0; i < numShaders; ++i)
	{
		GLuint shader;
		if (createShaderFromFile(types[i], fileNames[i], shader))
		{
			shaders.push_back(shader);
			glAttachShader(program, shader);
		}
		else
		{
			for (int j = 0; j < shaders.size(); ++j)
			{
				glDeleteShader(shaders[j]);
			}
			glDeleteProgram(program);
			return false;
		}
	}

	glLinkProgram(program);

	for (int i = 0; i < shaders.size(); ++i)
	{
		glDeleteShader(shaders[i]);
	}

	// check link status
	GLint result;
	int infoLogLength;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> msg(infoLogLength + 1);
		glGetProgramInfoLog(program, infoLogLength, NULL, &msg[0]);
		MGlobal::displayError(msg.data());
	}
	if (!result)
	{
		glDeleteProgram(program);
		return false;
	}

	programs.push_back(program);
	return true; // success
}


bool ShaderHelper::createShaderFromFile(GLenum type, const char *fileName, GLuint &shader)
{
	std::string shaderSrc;
	readWholeFile(fileName, shaderSrc);

	shader = glCreateShader(type);
	const char *srcRaw = shaderSrc.c_str();
	glShaderSource(shader, 1, &srcRaw, NULL);
	glCompileShader(shader);

	// Check for errors
	GLint status = GL_FALSE;
	int infoLogLength = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> msg(infoLogLength + 1);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &msg[0]);
		MGlobal::displayError(msg.data());
	}
	if (!status)
	{
		return false; // failed
	}

	return true; // success
}


GLint ShaderHelper::getUniformLocation(GLuint program, const char *_name, bool suppressError)
{
	std::string name(_name);
	std::unordered_map<GLuint, std::unordered_map<std::string, GLint> >::const_iterator it_program = uniformLocations.find(program);

	if (it_program == uniformLocations.end())
	{
		bool programExist = false;

		for (int i = 0; i < programs.size(); ++i)
		{
			if (program == programs[i])
			{
				programExist = true;
				break;
			}
		}

		if (!programExist)
		{
			MGlobal::displayError("Shader program doesn't exist");
			return -1;
		}
		else
		{
			GLint loc = glGetUniformLocation(program, _name);
			if (loc < 0 && !suppressError)
			{
				std::string msg(_name);
				msg += ": Uniform doesn't exist or inactive";
				MGlobal::displayError(msg.c_str());
				return -1;
			}
			uniformLocations[program][name] = loc;
			return loc;
		}
	}

	std::unordered_map<std::string, GLint>::const_iterator it = it_program->second.find(name);

	if (it == it_program->second.end())
	{
		GLint loc = glGetUniformLocation(program, _name);
		if (loc < 0 && !suppressError)
		{
			std::string msg(_name);
			msg += ": Uniform doesn't exist or inactive";
			MGlobal::displayError(msg.c_str());
			return -1;
		}
		uniformLocations[program][name] = loc;
		return loc;
	}
	else
	{
		return it->second;
	}
}


void ShaderHelper::cleanup()
{
	for (int i = 0; i < programs.size(); ++i)
	{
		glDeleteProgram(programs[i]);
	}
	programs.clear();
	uniformLocations.clear();
}


void ShaderHelper::deleteProgram(GLuint program)
{
	int idxProgramToRemove = -1;

	for (int i = 0; i < programs.size(); ++i)
	{
		if (programs[i] == program)
		{
			glDeleteProgram(program);
			idxProgramToRemove = i;
			break;
		}
	}

	if (idxProgramToRemove >= 0)
	{
		programs.erase(programs.begin() + idxProgramToRemove);
	}

	if (uniformLocations.size() > 0)
	{
		uniformLocations.erase(program);
	}
}