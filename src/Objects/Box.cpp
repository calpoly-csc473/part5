
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Box.hpp"

#include <glm/gtc/epsilon.hpp>


Box::Box(const glm::vec3 & a, const glm::vec3 & b)
	: min(a), max(b)
{}

float Box::Intersect(const Ray & ray) const
{
	float largestMin = std::numeric_limits<float>::lowest();
	float smallestMax = std::numeric_limits<float>::max();

	for (int i = 0; i < 3; ++ i)
	{
		if (ray.direction[i] == 0)
		{
			if (ray.origin[i] < min[i] || ray.origin[i] > max[i])
			{
				return -1;
			}
		}
		else
		{
			float tmin = (min[i] - ray.origin[i]) / ray.direction[i];
			float tmax = (max[i] - ray.origin[i]) / ray.direction[i];

			if (tmin > tmax)
				std::swap(tmin, tmax);

			largestMin = glm::max(tmin, largestMin);
			smallestMax = glm::min(tmax, smallestMax);
		}
	}
	
	if (largestMin > smallestMax)
		return -1;

	if (smallestMax < 0)
		return -1;

	if (largestMin > 0)
		return largestMin;
	else
		return smallestMax;
}

glm::vec3 Box::CalculateNormal(glm::vec3 const & intersectionPoint) const
{
	static const float epsilon = 0.001f;

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
