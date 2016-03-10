#include "CommonInclude.h"
#include <fstream>
#include <iostream>


bool readWholeFile(const char *fileName, std::string &ret_content)
{
	bool success = true;
	std::ifstream ifs(fileName, std::ifstream::in | std::ifstream::binary);

	if (ifs)
	{
		ifs.seekg(0, std::ifstream::end);
		int length = ifs.tellg();
		ifs.seekg(0, std::ifstream::beg);

		ret_content.resize(length);
		ifs.read(&ret_content[0], length);

		ifs.close();
	}
	else
	{
		std::cout << "Cannot open shader file: " << fileName << '\n';
		return !success;
	}

	return success;
}