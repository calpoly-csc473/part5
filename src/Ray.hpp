
#pragma once

#include <ostream>
#include <glm/glm.hpp>



std::ostream & operator << (std::ostream & stream, const glm::vec3 & vec)
{
	return stream << vec.x << " " << vec.y << " " << vec.z;
}

std::ostream & operator << (std::ostream & stream, const glm::vec4 & vec)
{
	return stream << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
}

class Ray
{

public:

	glm::vec3 origin;
	glm::vec3 direction;

	Ray()
	{
		this->origin = glm::vec3(0);
		this->direction = glm::vec3(0, 0, 1);
	}

	Ray(glm::vec3 const & origin, glm::vec3 const & direction)
	{
		this->origin = origin;
		this->direction = direction;
	}

	glm::vec3 GetPoint(const float time) const
	{
		return origin + time * direction;
	}

	bool operator == (Ray const & other) const
	{
		return origin == other.origin && direction == other.direction;
	}

	bool operator != (Ray const & other) const
	{
		return origin != other.origin && direction != other.direction;
	}

	friend std::ostream & operator << (std::ostream & stream, Ray const & ray)
	{
		return stream << "{" << ray.origin << "} -> {" << ray.direction << "}";
	}

};
