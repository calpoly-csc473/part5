
#pragma once

#include <vector>
#include <string>

#include "Scene.hpp"
#include "Params.hpp"


class RayTracer;

class Application
{

public:

	void ReadArguments(int argc, char ** argv);
	int Run();

protected:

	void RunCommands();
	void ParseExtraParams(size_t const StartIndex);
	void LoadPovrayScene();
	void PrintRayInfo(Scene * scene, int const x, int const y, bool const decoration);

	void RunCommandSceneInfo();
	void DrawSceneThreaded();

	void PrintUsage();

	std::vector<std::string> CommandArguments;

	Scene * scene = nullptr;
	RayTracer * rayTracer = nullptr;

	Params params;
	std::string fileName;

};
