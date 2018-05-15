
#pragma once

#include "Object.hpp"


class Sphere : public Object
{

public:

	Sphere(const glm::vec3 & c, const float r);
	bool Intersect(Ray const & ray, RayHitResults * const HitInformation) const;
	std::string GetObjectType() const;

protected:

	float radius;
	glm::vec3 center;

};
