#ifndef COMMON_INCLUDE_H
#define COMMON_INCLUDE_H

#include <unordered_map>
#include <limits>
#include <memory>
#include <string>
#include <functional>
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
//#define SHADER_DIR "C:/Users/Alexander/Desktop/CIS660/FastCatAuthoringTool/FastCat/shaders"
#define SHADER_DIR "C:/Users/Jian Ru/Documents/CIS660/fastcat2013/FastCatVS2013/FastCat/shaders"

//#define FAST_CAT_DEBUG_MODE

bool readWholeFile(const char *fileName, std::string &ret_content);


// Hashing std::pair for use in std::unordered_map
template <class T>
inline void hash_combine(std::size_t &seed, const T &v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template<typename S, typename T>
struct HashPair
{
	inline std::size_t operator()(const std::pair<S, T> &v) const
	{
		std::size_t seed = 0;
		hash_combine(seed, v.first);
		hash_combine(seed, v.second);
		return seed;
	}
};

typedef std::unordered_map<std::pair<int, int>, float, HashPair<int, int> > EdgeSharpnessLUT;

#endif // COMMON_INCLUDE_H