
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#pragma once

#include <Scene\Scene.hpp>
#include <RayTracer\RayTracer.hpp>


class RayInfo
{

public:

	static void PrintRayInfo(RayTracer * rayTracer, Scene * scene, const int x, const int y, const bool decoration);

};
