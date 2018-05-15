
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

void Scene::SetParams(Params const & params)
{
	this->params = params;
	camera.params = params;

	if (params.useCookTorrance)
	{
		//brdf = new CCookTorranceBRDF();
	}
	else
	{
		//brdf = new CBlinnPhongBRDF();
	}
}

Params const & Scene::GetParams() const
{
	return params;
}

Camera const & Scene::GetCamera() const
{
	return camera;
}

Camera & Scene::GetCamera()
{
	return camera;
}

const BRDF * Scene::GetBRDF()
{
	return brdf;
}

bool Scene::IsLightOccluded(Object const * const HitObject, glm::vec3 const & Point, glm::vec3 const & LightPosition, PixelContext::Iteration * CurrentIteration) const
{
	const float shadowRayEpsilon = 0.0001f;

	if (params.useShadows)
	{
		glm::vec3 const LightVector = glm::normalize(LightPosition - Point);
		float const LightDistance = glm::length(LightPosition - Point);
		Ray const ray = Ray(Point, LightVector);

		RayHitResults ShadowHit;

		if (CurrentIteration)
		{
			CurrentIteration->shadowRays.push_back(PixelContext::ShadowInfo());
			CurrentIteration->shadowRays.back().ray = ray;
		}

		for (auto Object : objects)
		{
			if (Object != HitObject && Object->Intersect(ray, & ShadowHit))
			{
				if (ShadowHit.t && ShadowHit.t < LightDistance)
				{
					if (CurrentIteration)
					{
						CurrentIteration->shadowRays.back().Hit = true;
					}
					return true;
				}
			}
		}
	}

	return false;
}

RayHitResults Scene::GetRayHitResults(Ray const & ray) const
{
	RayHitResults Closest, CurrentHit;

	for (auto Object : objects)
	{
		if (Object->Intersect(ray, & CurrentHit))
		{
			if (! Closest.object || CurrentHit.t < Closest.t)
			{
				Closest = CurrentHit;
			}
		}
	}

	return Closest;
}
