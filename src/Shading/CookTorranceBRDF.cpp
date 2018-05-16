
#include "CookTorranceBRDF.hpp"

#include <glm/gtc/constants.hpp>


float sq(float const a)
{
	return a * a;
}

float saturate(float const a)
{
	return glm::clamp(a, 0.f, 1.f);
}


float CookTorranceBRDF::ChiPositive(float const x)
{
	return x > 0.f ? 1.f : 0.f;
}

float CookTorranceBRDF::G_GGX_Partial(glm::vec3 const & v, glm::vec3 const & g, glm::vec3 const & m, float const alpha)
{
	float chi = ChiPositive(glm::dot(v, m) / glm::dot(v, g));
	float tan2theta = (1.f - sq(glm::dot(v, g))) / sq(glm::dot(v, g));

	return chi * 2.f / (1.f + sqrt(1.f + sq(alpha) * tan2theta));
}

float CookTorranceBRDF::NDF_Beckmann(glm::vec3 const & H, glm::vec3 const & N, float const alpha)
{
	float const NormFactor = 1.f / (glm::pi<float>() * sq(alpha));

	float const HdotN = glm::clamp(glm::dot(H, N), 0.f, 1.f);

	if (HdotN == 0)
	{
		return 0.f;
	}

	return NormFactor * exp((sq(HdotN) - 1.f) / (sq(alpha * HdotN))) / (pow(HdotN, 4.f));
}

float CookTorranceBRDF::NDF_GGX(glm::vec3 const & H, glm::vec3 const & N, float const alpha)
{
	float HdotN = saturate(glm::dot(H, N));
	return sq(alpha) / (glm::pi<float>() * sq(sq(HdotN) * (sq(alpha) - 1) + 1));
}

float CookTorranceBRDF::G_CookTorrance(glm::vec3 const & H, glm::vec3 const & N, glm::vec3 const & V, glm::vec3 const & L)
{
	float HdotN = saturate(glm::dot(H, N));
	float NdotV = saturate(glm::dot(N, V));
	float NdotL = saturate(glm::dot(N, L));
	float VdotH = saturate(glm::dot(V, H));

	return glm::min(1.f, glm::min(2.f*HdotN*NdotV / VdotH, 2.f*HdotN*NdotL / VdotH));
}

float CookTorranceBRDF::G_GGX(glm::vec3 const & H, glm::vec3 const & N, glm::vec3 const & V, glm::vec3 const & L, float const alpha)
{
	float HdotN = saturate(glm::dot(H, N));
	float NdotV = saturate(glm::dot(N, V));
	float NdotL = saturate(glm::dot(N, L));
	float VdotH = saturate(glm::dot(V, H));

	return G_GGX_Partial(V, N, H, alpha) * G_GGX_Partial(L, N, H, alpha);
}

float CookTorranceBRDF::F_Schlick(glm::vec3 const & H, glm::vec3 const & V, float const IOR)
{
	float F0 = sq((IOR - 1.f) / (IOR + 1.f));
	return F0 + (1.f - F0) * pow(1.f - glm::dot(V, H), 5.f);
}

glm::vec3 CookTorranceBRDF::CalculateDiffuse(const Material & Material, const SurfaceVectors & surface) const
{
	return glm::vec3(saturate(glm::dot(surface.normal, surface.light)));
}

glm::vec3 CookTorranceBRDF::CalculateSpecular(const Material & material, const SurfaceVectors & surface) const
{
	glm::vec3 const Half = glm::normalize(surface.light + surface.view);
	float const Alpha = sq(material.finish.roughness);

	float const D = NDF_GGX(Half, surface.normal, Alpha);
	float const G = G_GGX(Half, surface.normal, surface.view, surface.light, Alpha);
	float const F = F_Schlick(Half, surface.view, material.finish.ior);

	float const Denom = 4.f * glm::max(1.f / 16.f, std::abs(glm::dot(surface.normal, surface.view)));

	return glm::vec3(D * G * F / Denom);
}
