
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Scene.hpp"


Object * Scene::AddObject(Object * object)
{
	objects.push_back(object);
	object->SetID((int) objects.size());
	return object;
}

Light * Scene::AddLight(Light * light)
{
	lights.push_back(light);
	return light;
}

const std::vector<Object *> & Scene::GetObjects() const
{
	return objects;
}

const std::vector<Light *> & Scene::GetLights() const
{
	return lights;
}

Camera const & Scene::GetCamera() const
{
	return camera;
}

Camera & Scene::GetCamera()
{
	return camera;
}

bool Scene::IsLightOccluded(const glm::vec3 & point, const glm::vec3 & lightPosition, PixelContext::Iteration * currentIteration) const
{
	const glm::vec3 lightVector = glm::normalize(lightPosition - point);
	const float lightDistance = glm::length(lightPosition - point);
	const Ray ray = Ray(point, lightVector);

	if (currentIteration)
	{
		currentIteration->shadowRays.push_back(PixelContext::ShadowInfo());
		currentIteration->shadowRays.back().ray = ray;
	}

	for (auto Object : objects)
	{
		const float t = Object->IntersectTransformed(ray);
		if (t >= 0.f && t < lightDistance)
		{
			if (currentIteration)
			{
				currentIteration->shadowRays.back().hit = true;
			}
			return true;
		}
	}

	return false;
}

RayHitResults Scene::GetRayHitResults(const Ray & ray) const
{
	RayHitResults results;

	for (const Object * object : objects)
	{
		const float t = object->IntersectTransformed(ray);
		if (t >= 0.f)
		{
			if (! results.object || t < results.t)
			{
				results.object = object;
				results.t = t;
			}
		}
	}

	if (results.object)
	{
		results.point = ray.GetPoint(results.t);
		results.normal = results.object->CalculateNormalTransformed(results.point);
	}

	return results;
}
