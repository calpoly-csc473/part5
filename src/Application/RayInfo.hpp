
#pragma once

#include <Scene/Scene.hpp>
#include <RayTracer/RayTracer.hpp>


class RayInfo
{

public:

	static void PrintRayInfo(RayTracer * rayTracer, Scene * scene, const int x, const int y, const bool decoration);

};
