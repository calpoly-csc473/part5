
#pragma once

#include <Scene/Object.hpp>


class Sphere : public Object
{

public:

	Sphere(const glm::vec3 & c, const float r);
	float Intersect(const Ray & ray) const;
	glm::vec3 CalculateNormal(glm::vec3 const & intersectionPoint) const;
	std::string GetObjectType() const;

protected:

	float radius;
	glm::vec3 center;

};
