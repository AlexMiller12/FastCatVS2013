#ifndef COMMMON_INCLUDE_H
#define COMMON_INCLUDE_H

#include <string>
#include "Dependencies/glew/glew.h"
#include "Dependencies/glfw/glfw3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SHADER_DIR "C:/Users/Jian Ru/Documents/CIS660/fastcat2013/FastCatVS2013/FastCat/shaders"


bool readWholeFile(const char *fileName, std::string &ret_content);

#endif // COMMON_INCLUDE_H