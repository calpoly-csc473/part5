
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#pragma once

#include <glm/glm.hpp>

#include "Object.hpp"
#include "Camera.hpp"
#include "Light.hpp"

#include <RayTracer/PixelContext.hpp>


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
	Scene(const Scene &) = delete;
	Scene & operator =(const Scene &) = delete;

public:

	Scene() = default;

	Object * AddObject(Object * Object);
	Light * AddLight(Light * Light);

	std::vector<Object *> & GetObjects();
	std::vector<Light *> & GetLights();

	const Camera & GetCamera() const;
	Camera & GetCamera();

	bool IsLightOccluded(const glm::vec3 & Point, const glm::vec3 & LightPosition, PixelContext::Iteration * CurrentIteration = nullptr) const;
	RayHitResults GetRayHitResults(const Ray & Ray) const;

protected:

	Camera camera;

	std::vector<Object *> objects;
	std::vector<Light *> lights;

};
