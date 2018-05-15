
#pragma once

#include <glm/glm.hpp>


struct Params
{
	glm::ivec2 WindowSize;

	bool UseShading = true;
	bool UseShadows = true;

	bool UseReflections = true;
	int ReflectDepth = 5;

	bool UseRefractions = true;
	int RefractDepth = 5;

	bool DebugNormals = false;
};
