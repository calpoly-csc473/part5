
#include "Scene.hpp"


void Scene::SetDebugContext(SPixelContext * Context)
{
	this->Context = Context;
}

CObject * Scene::AddObject(CObject * Object)
{
	Objects.push_back(Object);
	Object->SetID((uint) Objects.size());
	return Object;
}

CLight * Scene::AddLight(CLight * Light)
{
	Lights.push_back(Light);
	return Light;
}

std::vector<CObject *> & Scene::GetObjects()
{
	return Objects;
}

std::vector<CLight *> & Scene::GetLights()
{
	return Lights;
}

void Scene::SetParams(SSceneParams const & params)
{
	Params = params;
	Camera.Params = params;

	if (Params.UseCookTorrance)
	{
		BRDF = new CCookTorranceBRDF();
	}
	else
	{
		BRDF = new CBlinnPhongBRDF();
	}
}

SSceneParams const & Scene::GetParams() const
{
	return Params;
}

CCamera const & Scene::GetCamera() const
{
	return Camera;
}

CCamera & Scene::GetCamera()
{
	return Camera;
}
