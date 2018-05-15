
#pragma once

#include <glm/glm.hpp>


class Object;

struct RayHitResults
{
	float t = 0;
	glm::vec3 point;
	glm::vec3 normal;
	const Object * object = nullptr;
};
