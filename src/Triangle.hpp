
#pragma once

#include "Object.hpp"


class Triangle : public Object
{

public:

	Triangle(const glm::vec3 & v1, const glm::vec3 & v2, const glm::vec3 & v3);
	bool Intersect(Ray const & ray, RayHitResults * const hit) const;
	std::string GetObjectType() const;

protected:

	glm::vec3 a, b, c;
	glm::vec3 normal;

};
