
#include "Application.hpp"
#include "SceneInfo.hpp"
#include "Renderer.hpp"

#include "RayTracer.hpp"
#include "Scene.hpp"

#include "Sphere.hpp"
#include "Plane.hpp"
#include "Triangle.hpp"

#include <parser/Tokenizer.hpp>
#include <parser/Parser.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>


void Application::ReadArguments(int argc, char ** argv)
{
	for (int i = 0; i < argc; ++ i)
	{
		CommandArguments.push_back(argv[i]);
	}
}

int Application::Run()
{
	int ReturnStatus = 0;

	try
	{
		RunCommands();
		ReturnStatus = 0;
	}
	catch (std::exception const & e)
	{
		std::cout << e.what() << std::endl;
		PrintUsage();
		ReturnStatus = 1;
	}

	return ReturnStatus;
}

void Application::PrintUsage()
{
	if (! CommandArguments.size())
	{
		printf("No command arguments found.\n");
		printf("usage: raytracer <command> <file_name> [arguments ...]\n");
		return;
	}

	if (CommandArguments.size() < 3)
	{
		printf("usage: %s <command> <file_name> [arguments ...]\n", CommandArguments[0].c_str());
		return;
	}

	std::string const ProgramName = CommandArguments[0];
	std::string const Command = CommandArguments[1];

	if (Command == "render")
	{
		printf("usage: %s render <file_name> [arguments ...]\n", ProgramName.c_str());
	}
	else if (Command == "sceneinfo")
	{
		printf("usage: %s sceneinfo <file_name> [arguments ...]\n", ProgramName.c_str());
	}
	else if (Command == "pixelray")
	{
		printf("usage: %s pixelray <file_name> [arguments ...]\n", ProgramName.c_str());
	}
	else if (Command == "firsthit")
	{
		printf("usage: %s firsthit <file_name> [arguments ...]\n", ProgramName.c_str());
	}
	else
	{
		printf("unknown command: %s\n", Command.c_str());
	}
}

void Application::RunCommands()
{
	if (CommandArguments.size() < 2)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	std::string const ProgramName = CommandArguments[0];
	std::string const Command = CommandArguments[1];

	if (CommandArguments.size() < 3)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	fileName = CommandArguments[2];

	std::cout << std::setiosflags(std::ios::fixed);
	std::cout << std::setprecision(4);


	/////////////////////
	// Simple Commands //
	/////////////////////

	if (Command == "sceneinfo")
	{
		SceneInfo::RunCommand(fileName);
		return;
	}


	///////////////////
	// Draw Commands //
	///////////////////

	scene = LoadPovrayScene(fileName);
	rayTracer = new RayTracer(scene);

	if (CommandArguments.size() < 5)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	params.imageSize.x = std::stoi(CommandArguments[3]);
	params.imageSize.y = std::stoi(CommandArguments[4]);

	params.useShading = true;
	params.useShadows = true;

	if (Command == "render" || Command == "raycast")
	{
		if (Command == "render")
		{
		}
		else if (Command == "raycast")
		{
			params.useShading = false;
			params.useShadows = false;
		}

		ParseExtraParams(5);
		rayTracer->SetParams(params);
		Renderer::DrawThreaded(rayTracer, scene);

		return;
	}


	////////////////////
	// Pixel Commands //
	////////////////////

	if (CommandArguments.size() < 7)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	int const X = std::stoi(CommandArguments[5]);
	int const Y = std::stoi(CommandArguments[6]);

	ParseExtraParams(7);
	rayTracer->SetParams(params);

	if (Command == "pixelray")
	{
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), rayTracer->GetParams().imageSize);

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << ray << std::endl;
		return;
	}
	else if (Command == "firsthit")
	{
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), rayTracer->GetParams().imageSize);
		RayHitResults const results = scene->GetRayHitResults(scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), rayTracer->GetParams().imageSize));
		float const intersection = results.t;
		Object const * const object = results.object;

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << ray  << std::endl;

		if (object)
		{
			std::cout << "T = " << intersection << std::endl;
			std::cout << "Object Type: " << object->GetObjectType() << std::endl;
			std::cout << "Color: " << object->GetMaterial().color << std::endl;
		}
		else
		{
			std::cout << "No Hit" << std::endl;
		}
		return;
	}
	else if (Command == "pixelcolor")
	{
		Pixel const pixel = rayTracer->CastRaysForPixel(glm::ivec2(X, Y));
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), rayTracer->GetParams().imageSize);
		RayHitResults const Results = scene->GetRayHitResults(scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), params.imageSize));
		float const intersection = Results.t;
		Object const * const object = Results.object;

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << ray << std::endl;

		if (object)
		{
			std::cout << "T = " << intersection << std::endl;
			std::cout << "Object Type: " << object->GetObjectType() << std::endl;
			std::cout << "BRDF: " << "Blinn-Phong" << std::endl;
			std::cout << "Color: " << pixel << std::endl;
		}
		else
		{
			std::cout << "No Hit" << std::endl;
		}
		return;
	}
	else if (Command == "printrays")
	{
		PrintRayInfo(scene, X, Y, false);
		return;
	}
	else if (Command == "pixeltrace")
	{
		PrintRayInfo(scene, X, Y, true);
		return;
	}

	printf("unknown command: %s\n", Command.c_str());
	throw std::invalid_argument("Unknown command.");
}

void Application::ParseExtraParams(size_t const StartIndex)
{
	std::string Remainder;
	for (size_t i = StartIndex; i < CommandArguments.size(); ++ i)
	{
		std::string const & Arg = CommandArguments[i];
		if (Arg == "-altbrdf")
		{
			params.useCookTorrance = true;
		}
		else if (Arg == "-normals")
		{
			params.debugNormals = true;
		}
	}
}

Scene * Application::LoadPovrayScene(const std::string & fileName)
{
	Scene * scene = new Scene();

	parser::Parser p;
	p.Parse(fileName);

	scene->GetCamera().SetPosition(p.camera.location);
	scene->GetCamera().SetLookAt(p.camera.look_at);
	scene->GetCamera().SetUpVector(p.camera.up);
	scene->GetCamera().SetRightVector(p.camera.right);

	for (const parser::Object & o : p.objects)
	{
		Object * object = nullptr;

		switch (o.type)
		{
		case parser::Object::Type::Sphere:
			object = new Sphere(o.v1, o.s1);
			break;

		case parser::Object::Type::Plane:
			object = new Plane(o.v1, o.s1);
			break;

		case parser::Object::Type::Triangle:
			object = new Triangle(o.v1, o.v2, o.v3);
			break;

		case parser::Object::Type::Box:
			std::cerr << "Box objects not supported." << std::endl;
			break;

		case parser::Object::Type::Cone:
			std::cerr << "Cone objects not supported." << std::endl;
			break;
		}

		if (object)
		{
			object->GetMaterial().finish = o.attributes.finish;
			object->GetMaterial().color = glm::vec3(o.attributes.pigment.x, o.attributes.pigment.y, o.attributes.pigment.z);
			object->GetMaterial().filter = o.attributes.pigment.w;
			scene->AddObject(object);
		}
	}

	for (const parser::Light & l : p.lights)
	{
		Light * light = new Light();
		light->color = l.color;
		light->position = l.position;
		scene->AddLight(light);
	}

	return scene;
}

void Application::PrintRayInfo(Scene * scene, int const x, int const y, bool const decoration)
{
}
