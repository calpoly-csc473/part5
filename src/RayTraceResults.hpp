
#pragma once

#include <glm/vec3.hpp>


struct RayTraceResults
{
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Reflection;
	glm::vec3 Refraction;
	glm::vec3 IntersectionPoint;

	glm::vec3 ToColor() const
	{
		return Ambient + Diffuse + Specular + Reflection + Refraction;
	}
};
