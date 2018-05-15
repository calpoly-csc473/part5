
#pragma once

#include <vector>
#include <string>

#include "Scene.hpp"
#include "Params.hpp"


class Application
{

public:

	void ReadArguments(int argc, char ** argv);
	int Run();

protected:

	void RunCommands();
	void ParseExtraParams(size_t const StartIndex);
	void LoadPovrayScene();

	void RunCommandSceneInfo();
	void DrawSceneThreaded();

	void PrintUsage();

	std::vector<std::string> CommandArguments;

	Scene * scene = nullptr;

	Params params;
	std::string fileName;

};
