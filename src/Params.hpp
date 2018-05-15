
#pragma once

#include <glm/glm.hpp>


struct Params
{
	glm::ivec2 imageSize;

	bool useShading = true;
	bool useShadows = true;
	bool useCookTorrance = false;

	bool useReflections = true;
	bool useRefractions = true;

	int recursiveDepth = 6;

	bool debugNormals = false;
};
