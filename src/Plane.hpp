
#pragma once

#include "Object.hpp"


class Plane : public Object
{

public:

	Plane(const glm::vec3 & n, const float d);
	bool Intersect(const Ray & ray, RayHitResults * const hit) const;
	std::string GetObjectType() const;

protected:

	glm::vec3 normal;
	float distance;

};
