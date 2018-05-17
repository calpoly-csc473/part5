
// Copyright (C) 2018 Ian Dunn
// For conditions of distribution and use, see the LICENSE file


#include "Application.hpp"
#include "SceneInfo.hpp"
#include "Renderer.hpp"
#include "RayInfo.hpp"

#include <RayTracer/RayTracer.hpp>
#include <Scene/Scene.hpp>

#include <Objects/Sphere.hpp>
#include <Objects/Plane.hpp>
#include <Objects/Triangle.hpp>

#include <parser/Tokenizer.hpp>
#include <parser/Parser.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdexcept>


void Application::ReadArguments(int argc, char ** argv)
{
	for (int i = 0; i < argc; ++ i)
	{
		commandArguments.push_back(argv[i]);
	}
}

int Application::Run()
{
	try
	{
		RunCommands();
		return 0;
	}
	catch (std::exception const & e)
	{
		std::cout << e.what() << std::endl;
		PrintUsage();
		return 1;
	}
}

void Application::PrintUsage()
{
	if (! commandArguments.size())
	{
		printf("No command arguments found.\n");
		printf("usage: raytracer <command> <file_name> [arguments ...]\n");
		return;
	}

	if (commandArguments.size() < 2)
	{
		printf("usage: %s <command> <file_name> [arguments ...]\n", commandArguments[0].c_str());
		return;
	}

	const std::string programName = commandArguments[0];
	std::string command = commandArguments[1];

	if (command == "help" && commandArguments.size() > 2)
	{
		command = commandArguments[2];
	}

	bool showParamOptions = false;

	if (command == "render" || command == "raycast")
	{
		printf("usage: %s %s <file_name> <width> <height> [arguments ...]\n", programName.c_str(), command.c_str());
		showParamOptions = true;
	}
	else if (command == "sceneinfo")
	{
		printf("usage: %s sceneinfo <file_name>\n", programName.c_str());
	}
	else if (command == "pixelray" || command == "firsthit" || command == "printrays" || command == "pixeltrace")
	{
		printf("usage: %s %s <file_name> <width> <height> <x> <y> [arguments ...]\n", programName.c_str(), command.c_str());
		showParamOptions = true;
	}
	else if (command == "help")
	{
		printf("usage: raytracer <command> <file_name> [arguments ...]\n");
		printf("    supported commands:\n");
		printf("        raycast      render an image using a simple raycast\n");
		printf("        render       render a full, shaded raytrace\n");
		printf("        sceneinfo    simply print the objects found in the given scene description file\n");
		printf("        pixelray     print the camera-generated ray for the given pixel\n");
		printf("        firsthit     print the first object hit for the given pixel\n");
		printf("        printrays    print all recursively generated rays for the given pixel\n");
		printf("        pixeltrace   print all recursively generated rays for the given pixel, with fancy formatting\n");
	}
	else
	{
		printf("unknown command: %s\n", command.c_str());
	}

	if (showParamOptions)
	{
		printf("    possible arguments:\n");
		printf("        -altbrdf    use Cook-Torrance instead of Blinn-Phong BRDF\n");
		printf("        -normals    display surface normals instead of any shading\n");
	}
}

void Application::RunCommands()
{
	if (commandArguments.size() < 2)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	const std::string command = commandArguments[1];

	if (command == "help")
	{
		PrintUsage();
		return;
	}

	if (commandArguments.size() < 3)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	fileName = commandArguments[2];

	std::cout << std::setiosflags(std::ios::fixed);
	std::cout << std::setprecision(4);


	/////////////////////
	// Simple Commands //
	/////////////////////

	if (command == "sceneinfo")
	{
		SceneInfo::RunCommand(fileName);
		return;
	}


	///////////////////
	// Draw Commands //
	///////////////////

	scene = LoadPovrayScene(fileName);
	rayTracer = new RayTracer(scene);

	if (commandArguments.size() < 5)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	params.imageSize.x = std::stoi(commandArguments[3]);
	params.imageSize.y = std::stoi(commandArguments[4]);

	params.useShading = true;
	params.useShadows = true;

	if (command == "render" || command == "raycast")
	{
		if (command == "render")
		{
		}
		else if (command == "raycast")
		{
			params.useShading = false;
			params.useShadows = false;
		}

		ParseExtraParams(5);
		rayTracer->SetParams(params);
		Renderer::Draw(rayTracer);

		return;
	}


	////////////////////
	// Pixel Commands //
	////////////////////

	if (commandArguments.size() < 7)
	{
		throw std::invalid_argument("Insufficient arguments.");
	}

	const int x = std::stoi(commandArguments[5]);
	const int y = std::stoi(commandArguments[6]);

	ParseExtraParams(7);
	rayTracer->SetParams(params);

	if (command == "pixelray")
	{
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(x, y), rayTracer->GetParams().imageSize);

		std::cout << "Pixel: [" << x << ", " << y << "] Ray: " << ray << std::endl;
		return;
	}
	else if (command == "firsthit")
	{
		RayInfo::FirstHit(rayTracer, scene, x, y);
		return;
	}
	else if (command == "pixelcolor")
	{
		RayInfo::PixelColor(rayTracer, scene, x, y);
		return;
	}
	else if (command == "printrays")
	{
		RayInfo::DiagnosticTrace(rayTracer, scene, x, y, false);
		return;
	}
	else if (command == "pixeltrace")
	{
		RayInfo::DiagnosticTrace(rayTracer, scene, x, y, true);
		return;
	}

	throw std::invalid_argument("Unknown command.");
}

void Application::ParseExtraParams(size_t const StartIndex)
{
	for (size_t i = StartIndex; i < commandArguments.size(); ++ i)
	{
		std::string const & Arg = commandArguments[i];
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
		light->color = glm::vec3(l.color.x, l.color.y, l.color.z);
		light->position = l.position;
		scene->AddLight(light);
	}

	return scene;
}
