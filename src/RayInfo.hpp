
#pragma once

#include "RayTracer.hpp"
#include "Scene.hpp"


class RayInfo
{

public:

	static void PrintRayInfo(RayTracer * rayTracer, Scene * scene, const int x, const int y, const bool decoration);

};
