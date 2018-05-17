
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#pragma once

#include <glm/glm.hpp>

#include <Shading/BRDF.hpp>

#include <Scene/Object.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Light.hpp>
#include <Scene/Scene.hpp>

#include "PixelContext.hpp"
#include "Params.hpp"
#include "Pixel.hpp"
#include "PixelContext.hpp"
#include "RayTraceResults.hpp"



struct LightingResults
{
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class RayTracer
{

public:

	RayTracer(const Scene * scene);

	void SetDebugContext(PixelContext * context);
	void SetParams(const Params & params);
	const Params & GetParams() const;

	Pixel CastRaysForPixel(const glm::ivec2 & Pixel) const;
	RayTraceResults CastRay(const Ray & ray, const int depth) const;
	glm::vec3 GetAmbientResults(const Object * const HitObject, glm::vec3 const & Point, glm::vec3 const & Normal, const int depth) const;
	LightingResults GetLightingResults(const Light * const Light, const Material & Material, const glm::vec3 & Point, const glm::vec3 & View, const glm::vec3 & normal) const;
	glm::vec3 GetReflectionResults(const glm::vec3 & point, const glm::vec3 & reflection, const int Depth, PixelContext::Iteration * currentIteration = nullptr) const;
	glm::vec3 GetRefractionResults(const Material & material, const glm::vec3 & point, const glm::vec3 & refractionVector, const bool entering, const int depth, PixelContext::Iteration * currentIteration = nullptr) const;

	static glm::vec3 CalculateRefractionVector(glm::vec3 const & View, glm::vec3 Normal, float const IndexOfRefraction);

protected:

	Params params;
	const Scene * scene = nullptr;
	BRDF * brdf = nullptr;

	PixelContext * context = nullptr;

	const float reflectionEpsilon = 0.001f;
	const float refractionEpsilon = 0.001f;

};
