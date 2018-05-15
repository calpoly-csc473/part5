
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
