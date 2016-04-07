#ifndef COMMMON_INCLUDE_H
#define COMMON_INCLUDE_H

#include <limits>
#include <memory>
#include <string>
#include "Dependencies/glew/glew.h"
#include "Dependencies/glfw/glfw3.h"
#include "Dependencies/glm/glm.hpp"
#include "Dependencies/glm/gtc/matrix_transform.hpp"
#include "Dependencies/glm/gtc/type_ptr.hpp"
#include "Dependencies/glm/gtx/transform.hpp"

//#define NDEBUG // uncomment this line to disable assert
#include <assert.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SHADER_DIR "C:/Users/Alexander/Desktop/CIS660/FastCatAuthoringTool/FastCat/shaders"
//#define SHADER_DIR "C:/Users/Jian Ru/Documents/CIS660/fastcat2013/FastCatVS2013/FastCat/shaders"

//#define FAST_CAT_DEBUG_MODE

bool readWholeFile(const char *fileName, std::string &ret_content);

#endif // COMMON_INCLUDE_H