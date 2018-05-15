
#pragma once

#include <glm/glm.hpp>

#include "BRDF.h"

// #include "SPixel.h"
// #include "CObject.h"
// #include "CBoundingVolume.h"
// #include "CCamera.h"
// #include "CLight.h"
// #include "IBRDF.h"

// #include "SSceneParams.h"
// #include "SRayTraceResults.h"
// #include "SRayHitResults.h"
// #include "SRecursiveDepth.h"
// #include "SRecursiveDepth.h"



struct Pixel
{
	Pixel(unsigned char const r, unsigned char const g, unsigned char const b);
	Pixel(glm::vec3 const & v);

	bool operator == (ion::color3i const & Color) const;
	friend std::ostream & operator << (std::ostream & stream, Pixel const & pixel);

	unsigned char Red, Green, Blue;
};

struct LightingResults
{
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

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

class RayTracer
{

public:

	void SetDebugContext(SPixelContext * Context = nullptr);

	Pixel CastRaysForPixel(ion::vec2i const & Pixel) const;
	RayTraceResults CastRay(ion::ray3f const & Ray, int Depth) const;
	glm::vec3 GetAmbientResults(CObject const * const HitObject, glm::vec3 const & Point, glm::vec3 const & Normal, int Depth) const;
	LightingResults GetLightingResults(CLight const * const Light, SMaterial const & Material, glm::vec3 const & Point, glm::vec3 const & View, glm::vec3 const & Normal) const;
	glm::vec3 GetReflectionResults(glm::vec3 const & Point, glm::vec3 const & Reflection, int Depth, SPixelContext::SIteration * CurrentIteration = nullptr) const;
	glm::vec3 GetRefractionResults(SMaterial const & Material, glm::vec3 const & Point, glm::vec3 const & Refraction, bool const Entering, int Depth, SPixelContext::SIteration * CurrentIteration = nullptr) const;

protected:

	PixelContext * context = nullptr;

	BRDF * brdf = nullptr;

};
