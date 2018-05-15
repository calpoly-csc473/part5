
#include "Plane.hpp"


Plane::Plane(const glm::vec3 & n, const float d)
 : normal(n), distance(d)
{}

bool Plane::Intersect(const Ray & ray, RayHitResults * const hit) const
{
	float const denominator = glm::dot(ray.direction, normal);

	if (denominator != 0.f)
	{
		const float numerator = distance - glm::dot(ray.origin, normal);
		const float t = numerator / denominator;

		if (t >= 0)
		{
			hit->t = t;
			hit->point = ray.GetPoint(t);
			hit->normal = normal;
			hit->object = this;

			return true;
		}
	}

	return false;
}

std::string Plane::GetObjectType() const
{
	return "Plane";
}
