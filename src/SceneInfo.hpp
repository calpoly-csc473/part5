
#pragma once

#include <parser/Parser.hpp>
#include <string>


class SceneInfo
{

public:

	static void RunCommand(const std::string & fileName);
	static void PrintInfo(const parser::Parser & p);

};
