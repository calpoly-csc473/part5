
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

std::vector<Object *> & Scene::GetObjects()
{
	return objects;
}

std::vector<Light *> & Scene::GetLights()
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

bool Scene::IsLightOccluded(const Object * const HitObject, glm::vec3 const & Point, glm::vec3 const & LightPosition, PixelContext::Iteration * CurrentIteration) const
{
	const float shadowRayEpsilon = 0.0001f;

	glm::vec3 const LightVector = glm::normalize(LightPosition - Point);
	float const LightDistance = glm::length(LightPosition - Point);
	Ray const ray = Ray(Point, LightVector);

	if (CurrentIteration)
	{
		CurrentIteration->shadowRays.push_back(PixelContext::ShadowInfo());
		CurrentIteration->shadowRays.back().ray = ray;
	}

	for (auto Object : objects)
	{
		if (Object != HitObject)
		{
			const float t = Object->Intersect(ray);
			if (t >= 0.f && t < LightDistance)
			{
				if (CurrentIteration)
				{
					CurrentIteration->shadowRays.back().Hit = true;
				}
				return true;
			}
		}
	}

	return false;
}

RayHitResults Scene::GetRayHitResults(Ray const & ray) const
{
	RayHitResults results;

	for (const Object * object : objects)
	{
		const float t = object->Intersect(ray);
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
		results.normal = results.object->CalculateNormal(results.point);
	}

	return results;
}
