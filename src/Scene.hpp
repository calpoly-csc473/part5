
#pragma once

#include <glm/glm.hpp>


#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "PixelContext.hpp"


struct RayHitResults
{
	float t = 0;
	glm::vec3 point;
	glm::vec3 normal;
	const Object * object = nullptr;
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

	const Camera & GetCamera() const;
	Camera & GetCamera();

	bool IsLightOccluded(const Object * const HitObject, glm::vec3 const & Point, glm::vec3 const & LightPosition, PixelContext::Iteration * CurrentIteration = nullptr) const;
	RayHitResults GetRayHitResults(Ray const & Ray) const;

protected:

	Camera camera;

	std::vector<Object *> objects;
	std::vector<Light *> lights;

};
