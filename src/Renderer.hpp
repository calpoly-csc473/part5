
#pragma once

#include "RayTracer.hpp"
#include "Scene.hpp"


class Renderer
{

public:

	static void DrawThreaded(RayTracer * rayTracer, Scene * scene);

};
