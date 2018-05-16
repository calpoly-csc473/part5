
#include "BlinnPhongBRDF.hpp"


glm::vec3 BlinnPhongBRDF::CalculateDiffuse(const Material & Material, const SurfaceVectors & surface) const
{
	return glm::vec3(glm::clamp(glm::dot(surface.normal, surface.light), 0.f, 1.f));
}

glm::vec3 BlinnPhongBRDF::CalculateSpecular(const Material & Material, const SurfaceVectors & surface) const
{
	glm::vec3 const half = normalize(surface.light + surface.view);
	float const power = (2.f / (Material.finish.roughness * Material.finish.roughness) - 2.f);

	return glm::vec3(glm::pow(glm::clamp(glm::dot(surface.normal, half), 0.f, 1.f), power));
}
