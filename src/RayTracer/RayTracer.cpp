
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "RayTracer.hpp"

#include <Scene/Scene.hpp>
#include <Shading/BlinnPhongBRDF.hpp>
#include <Shading/CookTorranceBRDF.hpp>


RayTracer::RayTracer(Scene * scene)
{
	this->scene = scene;
}

void RayTracer::SetDebugContext(PixelContext * context)
{
	this->context = context;
}

void RayTracer::SetParams(Params const & params)
{
	this->params = params;

	if (params.useCookTorrance)
	{
		brdf = new CookTorranceBRDF();
	}
	else
	{
		brdf = new BlinnPhongBRDF();
	}
}

const Params & RayTracer::GetParams() const
{
	return params;
}

Pixel RayTracer::CastRaysForPixel(glm::ivec2 const & pixel) const
{
	Ray const ray = scene->GetCamera().GetPixelRay(pixel, params.imageSize);

	if (context)
	{
		context->iterations.push_back(new PixelContext::Iteration());
		context->iterations.back()->type = PixelContext::IterationType::Primary;
		context->iterations.back()->parent = nullptr;
	}

	return Pixel(CastRay(ray, params.recursiveDepth).ToColor());
}

RayTraceResults RayTracer::CastRay(Ray const & ray, int const Depth) const
{
	RayTraceResults Results;

	RayHitResults const HitResults = scene->GetRayHitResults(ray);
	Object const * HitObject = HitResults.object;

	PixelContext::Iteration * ContextIteration = nullptr;

	if (context)
	{
		ContextIteration = context->iterations.back();
		ContextIteration->ray = ray;
	}

	if (HitObject)
	{
		Material const & material = HitObject->GetMaterial();

		/////////////
		// Vectors //
		/////////////

		glm::vec3 const Point = Results.intersectionPoint = HitResults.point;
		glm::vec3 const View = glm::normalize(ray.direction) * -1.f;
		glm::vec3 const SurfaceNormal = glm::normalize(HitResults.normal);

		glm::vec3 Normal = SurfaceNormal;
		bool Entering = true;
		if (glm::dot(Normal, View) < 0.0)
		{
			Normal = -Normal;
			Entering = false;
		}
		glm::vec3 const Reflection = glm::normalize(Normal * glm::dot(View, Normal) * 2.f - View);


		///////////////////
		// Contributions //
		///////////////////

		float const LocalContribution = (1.f - material.filter) * (1.f - material.finish.reflection);
		float TransmissionContribution = (material.filter);
		float ReflectionContribution = (1.f - material.filter) * (material.finish.reflection);


		///////////////////
		// Local Shading //
		///////////////////

		Results.ambient = LocalContribution * GetAmbientResults(HitObject, Point, Normal, Depth);

		for (Light * light : scene->GetLights())
		{
			const bool InShadow = params.useShadows ? scene->IsLightOccluded(HitObject, Point, light->position, ContextIteration) : false;
			if (! InShadow)
			{
				const LightingResults lighting = GetLightingResults(light, HitObject->GetMaterial(), Point, View, Normal);

				Results.diffuse += LocalContribution * lighting.Diffuse;
				Results.specular += LocalContribution * lighting.Specular;
			}
		}


		////////////////
		// Refraction //
		////////////////

		if (params.useRefractions && TransmissionContribution > 0.f)
		{
			glm::vec3 const refractionVector = CalculateRefractionVector(View, SurfaceNormal, material.finish.ior);

			if (std::isnan(refractionVector.x) || std::isnan(refractionVector.y) || std::isnan(refractionVector.z))
			{
				ReflectionContribution += TransmissionContribution;
				TransmissionContribution = 0.f;
				if (ContextIteration)
				{
					ContextIteration->extraInfo += " total-internal-reflection";
				}
			}
			else
			{
				glm::vec3 const transmissionColor = GetRefractionResults(material, Point, refractionVector, Entering, Depth, ContextIteration);
				Results.refraction = material.filter * transmissionColor;
			}
		}


		/////////////////
		// Reflections //
		/////////////////

		if (params.useReflections && ReflectionContribution > 0.f)
		{
			glm::vec3 const ReflectionColor = material.color * GetReflectionResults(Point, Reflection, Depth, ContextIteration);
			Results.reflection = ReflectionContribution * ReflectionColor;
		}

		if (ContextIteration)
		{
			ContextIteration->hitObject = HitResults.object;
			ContextIteration->hitNormal = HitResults.normal;
			ContextIteration->hitTime = HitResults.t;
			ContextIteration->results = Results;
			ContextIteration->contributions = glm::vec3(LocalContribution, ReflectionContribution, TransmissionContribution);
		}

		if (params.debugNormals)
		{
			Results.ambient = glm::vec3(0.f);
			Results.specular = glm::vec3(0.f);
			Results.reflection = glm::vec3(0.f);
			Results.refraction = glm::vec3(0.f);

			Results.diffuse = Normal / 2.f + 0.5f;
		}
	}

	return Results;
}

glm::vec3 RayTracer::GetAmbientResults(const Object * const hitObject, glm::vec3 const & Point, glm::vec3 const & Normal, int Depth) const
{
	if (params.useShading)
	{
		return hitObject->GetMaterial().finish.ambient * hitObject->GetMaterial().color;
	}
	else
	{
		return glm::vec3(0.f);
	}
}

LightingResults RayTracer::GetLightingResults(Light const * const light, Material const & material, glm::vec3 const & Point, glm::vec3 const & View, glm::vec3 const & Normal) const
{
	LightingResults Results;

	if (params.useShading)
	{
		SurfaceVectors surface;
		surface.normal = Normal;
		surface.view = View;
		surface.light = glm::normalize(light->position - Point);

		if (brdf)
		{
			Results.Diffuse  = light->color * material.finish.diffuse  * material.color * brdf->CalculateDiffuse(material, surface);
			Results.Specular = light->color * material.finish.specular * material.color * brdf->CalculateSpecular(material, surface);
		}
	}
	else
	{
		Results.Diffuse = material.color;
	}

	return Results;
}

glm::vec3 RayTracer::GetReflectionResults(glm::vec3 const & Point, glm::vec3 const & Reflection, int Depth, PixelContext::Iteration * CurrentIteration) const
{
	if (Depth > 0)
	{
		if (context)
		{
			context->iterations.push_back(new PixelContext::Iteration());
			context->iterations.back()->type = PixelContext::IterationType::Reflection;
			context->iterations.back()->parent = CurrentIteration;
		}

		return CastRay(Ray(Point + reflectionEpsilon * Reflection, Reflection), Depth - 1).ToColor();
	}

	return glm::vec3(0.f);
}

glm::vec3 RayTracer::GetRefractionResults(Material const & Material, glm::vec3 const & Point, glm::vec3 const & Refraction, bool const Entering, int const Depth, PixelContext::Iteration * CurrentIteration) const
{
	if (Depth > 0)
	{
		if (context)
		{
			context->iterations.push_back(new PixelContext::Iteration());
			context->iterations.back()->type = PixelContext::IterationType::Refraction;
			context->iterations.back()->parent = CurrentIteration;

			if (Entering)
			{
				context->iterations.back()->extraInfo += " into-object";
			}
			else
			{
				context->iterations.back()->extraInfo += " into-air";
			}
		}

		RayTraceResults const results = CastRay(Ray(Point + refractionEpsilon * Refraction, Refraction), Depth - 1);

		if (! Entering)
		{
			return results.ToColor();
		}
		else
		{
			return results.ToColor() * Material.color;
		}
	}

	return glm::vec3(0.f);
}

glm::vec3 RayTracer::CalculateRefractionVector(glm::vec3 const & View, glm::vec3 Normal, float const IndexOfRefraction)
{
	float iorRatio;

	if (glm::dot(Normal, -View) < 0.f)
	{
		iorRatio = 1.f / IndexOfRefraction;
	}
	else
	{
		iorRatio = IndexOfRefraction / 1.f;
		Normal = -Normal;
	}

	const float c1 = glm::dot(View, Normal);
	const float c2 = sqrt(1 - (iorRatio * iorRatio) * (1 - (c1 * c1)));

	return glm::normalize((-View * iorRatio) + Normal * (iorRatio * c1 - c2));
}
