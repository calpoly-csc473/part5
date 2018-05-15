
#include "Sphere.hpp"


Sphere::Sphere(const glm::vec3 & c, const float r)
 : center(c), radius(r)
{}

bool Sphere::Intersect(Ray const & ray, RayHitResults * const HitInformation) const
{
	const glm::vec3 offset = ray.origin - center;
	const float A = glm::dot(ray.direction, ray.direction);
	const float B = glm::dot(ray.direction, offset);
	const float C = glm::dot(offset, offset) - radius*radius;

	const float discriminant = B * B - A * C;

	if (discriminant >= 0.f)
	{
		float dividend;
		if (B < 0)
		{
			dividend = -B + sqrt(discriminant);
		}
		else
		{
			dividend = -B - sqrt(discriminant);
		}

		float t = C / dividend;

		if (t <= 0.f)
		{
			t = dividend / A;
		}

		if (t >= 0.f)
		{
			HitInformation->t = t;
			HitInformation->point = ray.origin + ray.direction * t;
			HitInformation->normal = glm::normalize(HitInformation->point - center);
			HitInformation->object = this;

			return true;
		}
	}

	return false;
}

std::string Sphere::GetObjectType() const
{
	return "Sphere";
}
