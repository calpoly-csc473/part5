
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Box.hpp"

#include <glm/gtc/epsilon.hpp>


Box::Box(const glm::vec3 & a, const glm::vec3 & b)
	: min(a), max(b)
{}

float Box::Intersect(const Ray & ray) const
{
	float tmin = (min.x - ray.origin.x) / ray.direction.x;
	float tmax = (max.x - ray.origin.x) / ray.direction.x;

	if (tmin > tmax)
		std::swap(tmin, tmax);

	float tymin = (min.y - ray.origin.y) / ray.direction.y;
	float tymax = (max.y - ray.origin.y) / ray.direction.y;

	if (tymin > tymax)
		std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return -1;

	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - ray.origin.z) / ray.direction.z;
	float tzmax = (max.z - ray.origin.z) / ray.direction.z;

	if (tzmin > tzmax)
		std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return -1;

	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	if ((tmin > std::numeric_limits<float>::max()) || (tmax < 0))
		return -1;

	return tmin > 0 ? tmin : tmax;
}

glm::vec3 Box::CalculateNormal(glm::vec3 const & intersectionPoint) const
{
	static const float epsilon = 0.01f;

	glm::vec3 normal;

	if (glm::epsilonEqual(intersectionPoint.x, min.x, epsilon))
		normal = glm::vec3(-1, 0, 0);
	else if (glm::epsilonEqual(intersectionPoint.x, max.x, epsilon))
		normal = glm::vec3(1, 0, 0);
	else if (glm::epsilonEqual(intersectionPoint.y, min.y, epsilon))
		normal = glm::vec3(0, -1, 0);
	else if (glm::epsilonEqual(intersectionPoint.y, max.y, epsilon))
		normal = glm::vec3(0, 1, 0);
	else if (glm::epsilonEqual(intersectionPoint.z, min.z, epsilon))
		normal = glm::vec3(0, 0, -1);
	else if (glm::epsilonEqual(intersectionPoint.z, max.z, epsilon))
		normal = glm::vec3(0, 0, 1);

	return normal;
}

std::string Box::GetObjectType() const
{
	return "Box";
}
