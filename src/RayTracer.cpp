
#include "RayTracer.hpp"
#include "Scene.h"

using namespace ion;


SPixel RayTracer::CastRaysForPixel(vec2i const & Pixel) const
{
	color3f Color = 0;

	for (int i = 0; i < Params.SuperSampling; ++ i)
	{
		for (int j = 0; j < Params.SuperSampling; ++ j)
		{
			ray3f const Ray = Camera.GetPixelRay(Pixel, Params.WindowSize, vec2i(i, j), Params.SuperSampling);

			SRecursiveDepth Depth;
			Depth.ReflectDepth = Params.ReflectDepth;
			Depth.RefractDepth = Params.RefractDepth;
			Depth.GlobalIlluminationDepth = Params.GlobalIlluminationDepth;

			if (Context)
			{
				Context->Iterations.push_back(new SPixelContext::SIteration());
				Context->Iterations.back()->Type = SPixelContext::EIterationType::Primary;
				Context->Iterations.back()->Parent = nullptr;
			}
			Color += CastRay(Ray, Depth).ToColor();
		}
	}

	Color /= (float) Sq(Params.SuperSampling);

	return SPixel(Color);
}

SRayTraceResults RayTracer::CastRay(ray3f const & Ray, SRecursiveDepth const Depth) const
{
	SRayTraceResults Results;

	SRayHitResults const HitResults = GetRayHitResults(Ray);
	CObject const * HitObject = HitResults.Object;

	SPixelContext::SIteration * ContextIteration = nullptr;

	if (Context)
	{
		ContextIteration = Context->Iterations.back();
		ContextIteration->Ray = Ray;
	}

	if (HitObject)
	{
		SMaterial const & Material = HitObject->GetMaterial();

		/////////////
		// Vectors //
		/////////////

		vec3f const Point = Results.IntersectionPoint = HitResults.Point;
		vec3f const View = Ray.Direction.GetNormalized() * -1.f;
		vec3f const SurfaceNormal = HitResults.Normal.GetNormalized();

		vec3f Normal = SurfaceNormal;
		bool Entering = true;
		if (Dot(Normal, View) < 0.0)
		{
			Normal = -Normal;
			Entering = false;
		}
		vec3f const Reflection = Normalize(Normal * Dot(View, Normal) * 2.f - View);

		float FresnelReflectance = 0;
		if (Params.UseSchlicks)
		{
			FresnelReflectance = CRayTracerMath::SchlicksApproximation(HitObject->GetMaterial().IndexOfRefraction, Normal, View);
		}


		///////////////////
		// Contributions //
		///////////////////

		float const LocalContribution = (1.f - Material.Filter) * (1.f - Material.Reflection);
		float const TransmissionContribution = (Material.Filter) * (1.f - FresnelReflectance);
		float ReflectionContribution = (1.f - Material.Filter) * (Material.Reflection) + (Material.Filter) * (FresnelReflectance);


		///////////////////
		// Local Shading //
		///////////////////

		Results.Ambient = LocalContribution * GetAmbientResults(HitObject, Point, Normal, Depth);

		for (auto Light : Lights)
		{
			bool const InShadow = IsLightOccluded(HitObject, Point, Light->Position, ContextIteration);
			if (! InShadow)
			{
				SLightingResults LightingResults = GetLightingResults(Light, HitObject->GetMaterial(), Point, View, Normal);
				Results.Diffuse += LocalContribution * LightingResults.Diffuse;
				Results.Specular += LocalContribution * LightingResults.Specular;
			}
		}

		if (Params.UseAmbientOcclusion)
		{
			float const AO = CalculateAmbientOcclusion(Point, Normal);

			Results.Ambient *= AO;
			Results.Diffuse *= AO;
			Results.Specular *= AO;
		}


		////////////////
		// Refraction //
		////////////////

		if (Params.UseRefractions && TransmissionContribution > 0.f)
		{
			vec3f const RefractionVector = CRayTracerMath::CalculateRefractionVector(View, SurfaceNormal, Material.IndexOfRefraction);

			if (std::isnan(RefractionVector.X))
			{
				ReflectionContribution = (1.f - Material.Filter) * (Material.Reflection) + (Material.Filter);
				if (ContextIteration)
				{
					ContextIteration->ExtraInfo += " total-internal-reflection";
				}
			}
			else
			{
				color3f const TransmissionColor = GetRefractionResults(Material, Point, RefractionVector, Entering, Depth, ContextIteration);
				Results.Refraction = Material.Filter * TransmissionColor;
			}
		}


		/////////////////
		// Reflections //
		/////////////////

		if (Params.UseReflections && ReflectionContribution > 0.f)
		{
			color3f const ReflectionColor = Material.Color * GetReflectionResults(Point, Reflection, Depth, ContextIteration);
			Results.Reflection = ReflectionContribution * ReflectionColor;
		}

		if (ContextIteration)
		{
			ContextIteration->HitObject = HitResults.Object;
			ContextIteration->HitNormal = HitResults.Normal;
			ContextIteration->HitTime = HitResults.T;
			ContextIteration->RayTraceResults = Results;
			ContextIteration->Contributions = vec3f(LocalContribution, ReflectionContribution, TransmissionContribution);
		}

		if (Params.DebugNormals)
		{
			Results.Ambient = 0;
			Results.Specular = 0;
			Results.Reflection = 0;
			Results.Refraction = 0;

			Results.Diffuse = Normal / 2.f + 0.5f;
		}
	}

	return Results;
}

color3f RayTracer::GetAmbientResults(CObject const * const HitObject, vec3f const & Point, vec3f const & Normal, SRecursiveDepth Depth) const
{
	if (Params.UseShading)
	{
		return HitObject->GetMaterial().Ambient * HitObject->GetMaterial().Color;
	}
	else
	{
		return 0;
	}
}

SLightingResults RayTracer::GetLightingResults(CLight const * const Light, SMaterial const & Material, vec3f const & Point, vec3f const & View, vec3f const & Normal) const
{
	SLightingResults Results;

	if (Params.UseShading)
	{
		vec3f const LightPosition = Light->Position;
		vec3f const LightVector = (LightPosition - Point).GetNormalized();

		vec3f const Diffuse = BRDF->CalculateDiffuse(Material, LightVector, View, Normal);
		vec3f const Specular = BRDF->CalculateSpecular(Material, LightVector, View, Normal);

		Results.Diffuse = Light->Color * Material.Diffuse * Material.Color * Diffuse;
		Results.Specular = Light->Color * Material.Specular * Material.Color * Specular;
	}
	else
	{
		Results.Diffuse = Material.Color;
	}

	return Results;
}

color3f RayTracer::GetReflectionResults(vec3f const & Point, vec3f const & Reflection, SRecursiveDepth Depth, SPixelContext::SIteration * CurrentIteration) const
{
	if (Depth.ReflectDepth > 0)
	{
		if (Context)
		{
			Context->Iterations.push_back(new SPixelContext::SIteration());
			Context->Iterations.back()->Type = SPixelContext::EIterationType::Reflection;
			Context->Iterations.back()->Parent = CurrentIteration;
		}

		Depth.ReflectDepth --;
		return CastRay(ray3f(Point + ReflectionEpsilon * Reflection, Reflection), Depth).ToColor();
	}

	return 0;
}

color3f RayTracer::GetRefractionResults(SMaterial const & Material, vec3f const & Point, vec3f const & Refraction, bool const Entering, SRecursiveDepth Depth, SPixelContext::SIteration * CurrentIteration) const
{
	if (Depth.RefractDepth > 0)
	{
		if (Context)
		{
			Context->Iterations.push_back(new SPixelContext::SIteration());
			Context->Iterations.back()->Type = SPixelContext::EIterationType::Refraction;
			Context->Iterations.back()->Parent = CurrentIteration;

			if (Entering)
			{
				Context->Iterations.back()->ExtraInfo += " into-object";
			}
			else
			{
				Context->Iterations.back()->ExtraInfo += " into-air";
			}
		}

		Depth.RefractDepth --;
		SRayTraceResults const Results = CastRay(ray3f(Point + RefractionEpsilon * Refraction, Refraction), Depth);

		if (! Entering)
		{
			return Results.ToColor();
		}
		else
		{
			if (Params.UseBeers)
			{
				// Beer's Law
				float const Distance = Length(Point - Results.IntersectionPoint);

				color3f const Absorbance = (color3f(1.f) - Material.Color) * 0.15f * -Distance;
				color3f const Attenuation = color3f(expf(Absorbance.Red), expf(Absorbance.Green), expf(Absorbance.Blue));

				return Results.ToColor() * Attenuation;
			}
			else
			{
				return Results.ToColor() * Material.Color;
			}
		}
	}

	return 0;
}
