
#include "Application.hpp"

#include "RayTracer.hpp"
#include "Scene.hpp"

#include <parser/Tokenizer.hpp>
#include <parser/Parser.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <atomic>
#include <iomanip>
#include <thread>


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

	scene = new Scene();
	rayTracer = new RayTracer();

	params.imageSize.x = 640;
	params.imageSize.y = 480;

	std::cout << std::setiosflags(std::ios::fixed);
	std::cout << std::setprecision(4);


	/////////////////////
	// Simple Commands //
	/////////////////////

	if (Command == "sceneinfo")
	{
		RunCommandSceneInfo();
		return;
	}


	///////////////////
	// Draw Commands //
	///////////////////

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
		DrawSceneThreaded();

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
	scene->SetParams(params);
	LoadPovrayScene();

	if (Command == "pixelray")
	{
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), scene->GetParams().imageSize);

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << ray << std::endl;
		return;
	}
	else if (Command == "firsthit")
	{
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), scene->GetParams().imageSize);
		RayHitResults const results = scene->GetRayHitResults(scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), scene->GetParams().imageSize));
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
		Ray const ray = scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), scene->GetParams().imageSize);
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

void Application::LoadPovrayScene()
{
	std::ifstream file;
	file.open(fileName);

	if (! file.is_open())
	{
		std::cerr << "Failed to open file '" << fileName << "'\n" << std::endl;
		throw std::runtime_error("povray scene file could not be opened");
	}

	std::string Contents{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()}; // inefficient

	parser::Parser p;

	try
	{
		TokenStream ts = Tokenizer::Tokenize(Contents);
		p.Parse(ts);
	}
	catch (const std::exception & e)
	{
		std::cerr << "exception: " << e.what() << std::endl;
		throw std::runtime_error("povray scene could not be parsed");
	}
}

void Application::PrintRayInfo(Scene * scene, int const x, int const y, bool const decoration)
{
}

void Application::RunCommandSceneInfo()
{
	scene->SetParams(params);
	LoadPovrayScene();

	Camera const & camera = scene->GetCamera();

	std::cout << "Camera:" << std::endl;
	std::cout << "- Location: {" << camera.GetPosition() << "}" << std::endl;
	std::cout << "- Up: {" << camera.GetUpVector() << "}" << std::endl;
	std::cout << "- Right: {" << camera.GetRightVector() << "}" << std::endl;
	std::cout << "- Look at: {" << camera.GetLookAt() << "}" << std::endl;
	std::cout << std::endl;
	std::cout << "---" << std::endl;
	std::cout << std::endl;

	std::cout << scene->GetLights().size() << " light(s)" << std::endl;
	for (size_t i = 0; i < scene->GetLights().size(); ++ i)
	{
		std::cout << std::endl;
		std::cout << "Light[" << i << "]:" << std::endl;
		std::cout << "- Location: {" << scene->GetLights()[i]->position << "}" << std::endl;
		std::cout << "- Color: {" << scene->GetLights()[i]->color << "}" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "---" << std::endl;
	std::cout << std::endl;

	std::cout << scene->GetObjects().size() << " object(s)" << std::endl;
	for (size_t i = 0; i < scene->GetObjects().size(); ++ i)
	{
		std::cout << std::endl;
		std::cout << "Object[" << i << "]:" << std::endl;
		std::cout << "- Type: " << scene->GetObjects()[i]->GetObjectType() << std::endl;
		//std::cout << scene->GetObjects()[i]->GetObjectInfo();
	}
}

void Application::DrawSceneThreaded()
{
	bool const UseThreads = true;
	glm::ivec2 const ScreenSize = params.imageSize;

	unsigned char * ImageBuffer = new unsigned char[ScreenSize.x * ScreenSize.y * 4];
	for (int x = 0; x < ScreenSize.x; ++ x)
	{
		for (int y = 0; y < ScreenSize.y; ++ y)
		{
			ImageBuffer[x * 4 + y * 4 * ScreenSize.x + 0] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.x + 1] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.x + 2] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.x + 3] = 0;
		}
	}


	/////////////////
	// Scene Setup //
	/////////////////

	scene->SetParams(params);
	LoadPovrayScene();
	


	///////////////////
	// Render Thread //
	///////////////////

	static int const ThreadCount = 12;
	int const PixelCount = ScreenSize.x * ScreenSize.y;

	std::atomic<int> DoneCount;
	std::atomic<int> currentPixel;

	DoneCount = 0;
	currentPixel = 0;

	auto RenderKernel = [&](int const ThreadIndex)
	{
		while (true)
		{
			int pixel = currentPixel ++;

			if (pixel >= PixelCount)
			{
				break;
			}

			int const X = pixel / ScreenSize.y;
			int const Y = pixel % ScreenSize.y;

			Pixel p = rayTracer->CastRaysForPixel(glm::ivec2(X, Y));
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.x + 0] = p.Red;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.x + 1] = p.Green;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.x + 2] = p.Blue;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.x + 3] = 255;
		}

		DoneCount ++;

		if (ThreadIndex == 0)
		{
			while (DoneCount < ThreadCount)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	};

	std::vector<std::thread> Threads;
	for (int i = 0; i < ThreadCount; ++ i)
	{
		Threads.push_back(std::thread(RenderKernel, i));
	}
	for (int i = 0; i < ThreadCount; ++ i)
	{
		Threads[i].join();
	}

	//Image->Write("output.png");
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
