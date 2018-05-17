
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "RayTracer.hpp"

#include <Scene/Scene.hpp>
#include <Shading/BlinnPhongBRDF.hpp>
#include <Shading/CookTorranceBRDF.hpp>


RayTracer::RayTracer(const Scene * scene)
{
	this->scene = scene;
}

void RayTracer::SetDebugContext(PixelContext * context)
{
	this->context = context;
}

void RayTracer::SetParams(const Params & params)
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

Pixel RayTracer::CastRaysForPixel(const glm::ivec2 & pixel) const
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

RayTraceResults RayTracer::CastRay(const Ray & ray, const int depth) const
{
	const float surfaceEpsilon = 0.0001f;

	RayTraceResults results;

	const RayHitResults hitResults = scene->GetRayHitResults(ray);
	const Object * hitObject = hitResults.object;

	PixelContext::Iteration * contextIteration = nullptr;

	if (context)
	{
		contextIteration = context->iterations.back();
		contextIteration->ray = ray;
	}

	if (hitObject)
	{
		const Material & material = hitObject->GetMaterial();

		/////////////
		// Vectors //
		/////////////

		const glm::vec3 point = results.intersectionPoint = hitResults.point;
		const glm::vec3 view = -glm::normalize(ray.direction);
		const glm::vec3 normal = glm::normalize(hitResults.normal);
		const glm::vec3 reflection = glm::normalize(normal * glm::dot(view, normal) * 2.f - view);


		///////////////////
		// Contributions //
		///////////////////

		float const localContribution = (1.f - material.filter) * (1.f - material.finish.reflection);
		float transmissionContribution = (material.filter);
		float reflectionContribution = (1.f - material.filter) * (material.finish.reflection);


		///////////////////
		// Local Shading //
		///////////////////

		results.ambient = localContribution * GetAmbientResults(hitObject, point, normal, depth);

		for (Light * light : scene->GetLights())
		{

			const bool inShadow = params.useShadows ? scene->IsLightOccluded(point + normal*surfaceEpsilon, light->position, contextIteration) : false;
			if (! inShadow)
			{
				const LightingResults lighting = GetLightingResults(light, hitObject->GetMaterial(), point, view, normal);

				results.diffuse += localContribution * lighting.diffuse;
				results.specular += localContribution * lighting.specular;
			}
		}


		////////////////
		// Refraction //
		////////////////

		if (params.useRefractions && transmissionContribution > 0.f)
		{
			const glm::vec3 refractionVector = CalculateRefractionVector(view, normal, material.finish.ior);
			const bool entering = glm::dot(normal, view) >= 0.f;

			if (refractionVector == glm::vec3(0.f))
			{
				// CalculateRefractionVector returns the zero-vector in the case of total internal reflection

				reflectionContribution += transmissionContribution;
				transmissionContribution = 0.f;
				if (contextIteration)
				{
					contextIteration->extraInfo += " total-internal-reflection";
				}
			}
			else
			{
				const glm::vec3 transmissionColor = GetRefractionResults(material, entering ? point - normal*surfaceEpsilon : point + normal*surfaceEpsilon, refractionVector, entering, depth, contextIteration);
				results.refraction = material.filter * transmissionColor;
			}
		}


		/////////////////
		// Reflections //
		/////////////////

		if (params.useReflections && reflectionContribution > 0.f)
		{
			const glm::vec3 reflectionColor = material.color * GetReflectionResults(point + normal*surfaceEpsilon, reflection, depth, contextIteration);
			results.reflection = reflectionContribution * reflectionColor;
		}

		if (contextIteration)
		{
			contextIteration->hitObject = hitResults.object;
			contextIteration->hitNormal = hitResults.normal;
			contextIteration->hitTime = hitResults.t;
			contextIteration->results = results;
			contextIteration->contributions = glm::vec3(localContribution, reflectionContribution, transmissionContribution);
		}

		if (params.debugNormals)
		{
			results.ambient = glm::vec3(0.f);
			results.specular = glm::vec3(0.f);
			results.reflection = glm::vec3(0.f);
			results.refraction = glm::vec3(0.f);

			results.diffuse = normal / 2.f + 0.5f;
		}
	}

	return results;
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
			Results.diffuse  = light->color * material.finish.diffuse  * material.color * brdf->CalculateDiffuse(material, surface);
			Results.specular = light->color * material.finish.specular * material.color * brdf->CalculateSpecular(material, surface);
		}
	}
	else
	{
		Results.diffuse = material.color;
	}

	return Results;
}

glm::vec3 RayTracer::GetReflectionResults(glm::vec3 const & Point, const glm::vec3 & Reflection, int Depth, PixelContext::Iteration * currentIteration) const
{
	if (Depth > 0)
	{
		if (context)
		{
			context->iterations.push_back(new PixelContext::Iteration());
			context->iterations.back()->type = PixelContext::IterationType::Reflection;
			context->iterations.back()->parent = currentIteration;
		}

		return CastRay(Ray(Point + reflectionEpsilon * Reflection, Reflection), Depth - 1).ToColor();
	}

	return glm::vec3(0.f);
}

glm::vec3 RayTracer::GetRefractionResults(const Material & material, const glm::vec3 & point, glm::vec3 const & refractionVector, const bool entering, int const depth, PixelContext::Iteration * currentIteration) const
{
	if (depth > 0)
	{
		if (context)
		{
			context->iterations.push_back(new PixelContext::Iteration());
			context->iterations.back()->type = PixelContext::IterationType::Refraction;
			context->iterations.back()->parent = currentIteration;

			if (entering)
			{
				context->iterations.back()->extraInfo += " into-object";
			}
			else
			{
				context->iterations.back()->extraInfo += " into-air";
			}
		}

		const RayTraceResults results = CastRay(Ray(point, refractionVector), depth - 1);

		if (entering)
		{
			return results.ToColor() * material.color;
		}
		else
		{
			return results.ToColor();
		}
	}

	return glm::vec3(0.f);
}

glm::vec3 RayTracer::CalculateRefractionVector(const glm::vec3 & view, glm::vec3 normal, const float ior)
{
	float iorRatio;

	if (glm::dot(view, normal) >= 0.f)
	{
		iorRatio = 1.f / ior;
	}
	else
	{
		iorRatio = ior / 1.f;
		normal = -normal;
	}

	const float c1 = glm::dot(view, normal);
	const float c2 = 1 - (iorRatio * iorRatio) * (1 - (c1 * c1));

	if (c2 < 0.f)
	{
		return glm::vec3(0.f, 0.f, 0.f);
	}

	return glm::normalize((-view * iorRatio) + normal * (iorRatio * c1 - sqrt(c2)));
}
