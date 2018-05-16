
#pragma once

#include "Object.hpp"


class Triangle : public Object
{

public:

	Triangle(const glm::vec3 & v1, const glm::vec3 & v2, const glm::vec3 & v3);
	float Intersect(const Ray & ray) const;
	glm::vec3 CalculateNormal(glm::vec3 const & intersectionPoint) const;
	std::string GetObjectType() const;

protected:

	glm::vec3 a, b, c;
	glm::vec3 normal;

};
