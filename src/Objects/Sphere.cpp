
#include "Sphere.hpp"


Sphere::Sphere(const glm::vec3 & c, const float r)
 : center(c), radius(r)
{}

float Sphere::Intersect(const Ray & ray) const
{
	const glm::vec3 offset = ray.origin - center;
	const float a = glm::dot(ray.direction, ray.direction);
	const float b = glm::dot(ray.direction, offset);
	const float c = glm::dot(offset, offset) - radius*radius;

	const float discriminant = b * b - a * c;

	if (discriminant >= 0.f)
	{
		float dividend;
		if (b < 0)
		{
			dividend = -b + sqrt(discriminant);
		}
		else
		{
			dividend = -b - sqrt(discriminant);
		}

		float t = c / dividend;

		if (t <= 0.f)
		{
			t = dividend / a;
		}

		if (t >= 0.f)
		{
			return t;
		}
	}

	return -1;
}

glm::vec3 Sphere::CalculateNormal(glm::vec3 const & intersectionPoint) const
{
	return glm::normalize(intersectionPoint - center);
}

std::string Sphere::GetObjectType() const
{
	return "Sphere";
}
