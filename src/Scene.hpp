
#pragma once

#include <glm/glm.hpp>


#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"

#include "Params.hpp"
#include "PixelContext.hpp"
#include "RayTraceResults.hpp"
#include "RayHitResults.hpp"



struct LightingResults
{
	glm::vec3 Diffuse;
	glm::vec3 Specular;
};

class Scene
{

	// non-copyable
	Scene(Scene const &) = delete;
	Scene & operator =(Scene const &) = delete;

public:

	Scene() = default;

	Object * AddObject(Object * Object);
	Light * AddLight(Light * Light);

	std::vector<Object *> & GetObjects();
	std::vector<Light *> & GetLights();

	void SetParams(Params const & params);
	const Params & GetParams() const;

	const Camera & GetCamera() const;
	Camera & GetCamera();

	bool IsLightOccluded(const Object * const HitObject, glm::vec3 const & Point, glm::vec3 const & LightPosition, PixelContext::Iteration * CurrentIteration = nullptr) const;
	RayHitResults GetRayHitResults(Ray const & Ray) const;

protected:

	Params params;
	Camera camera;

	std::vector<Object *> Objects;
	std::vector<Light *> Lights;

	const float ShadowRayEpsilon = 0.0001f;
	const float ReflectionEpsilon = 0.001f;
	const float RefractionEpsilon = 0.001f;
	const float MonteCarloEpsilon = 0.001f;

};
