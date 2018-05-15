
#pragma once

#include <glm/glm.hpp>

#include "BRDF.hpp"
#include "PixelContext.hpp"

#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"

#include "Scene.hpp"
#include "Params.hpp"
#include "Pixel.hpp"
#include "PixelContext.hpp"
#include "RayTraceResults.hpp"
#include "RayHitResults.hpp"



struct LightingResults
{
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

class RayTracer
{

public:

	void SetDebugContext(PixelContext * Context = nullptr);

	Pixel CastRaysForPixel(glm::ivec2 const & Pixel) const;
	RayTraceResults CastRay(Ray const & Ray, int Depth) const;
	glm::vec3 GetAmbientResults(Object const * const HitObject, glm::vec3 const & Point, glm::vec3 const & Normal, int Depth) const;
	LightingResults GetLightingResults(Light const * const Light, Material const & Material, glm::vec3 const & Point, glm::vec3 const & View, glm::vec3 const & Normal) const;
	glm::vec3 GetReflectionResults(glm::vec3 const & Point, glm::vec3 const & Reflection, int Depth, PixelContext::Iteration * CurrentIteration = nullptr) const;
	glm::vec3 GetRefractionResults(Material const & Material, glm::vec3 const & Point, glm::vec3 const & Refraction, bool const Entering, int Depth, PixelContext::Iteration * CurrentIteration = nullptr) const;

	static glm::vec3 CalculateRefractionVector(glm::vec3 const & View, glm::vec3 Normal, float const IndexOfRefraction);

protected:

	PixelContext * context = nullptr;
	Scene * scene = nullptr;
	Params params;

	const float shadowRayEpsilon = 0.0001f;
	const float reflectionEpsilon = 0.001f;
	const float refractionEpsilon = 0.001f;

};
