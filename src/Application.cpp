
#include "Application.hpp"

#include <iostream>
#include <string>
#include <atomic>
#include <iterator>
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

	params.WindowSize.x = 640;
	params.WindowSize.y = 480;

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

	Params.WindowSize.X = std::stoi(CommandArguments[3]);
	Params.WindowSize.Y = std::stoi(CommandArguments[4]);

	Params.UseShading = true;
	Params.UseShadows = true;

	Params.BoundingVolumeHierarchy = false;
	Params.UseSchlicks = false;
	Params.UseBeers = false;

	if (Command == "render" || Command == "raycast")
	{
		if (Command == "render")
		{
		}
		else if (Command == "raycast")
		{
			Params.UseShading = false;
			Params.UseShadows = false;
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
	Scene->SetParams(params);
	LoadPovrayScene();

	if (Command == "pixelray")
	{
		ray3f const Ray = Scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), Scene->GetParams().WindowSize);

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << Ray << std::endl;
		return;
	}
	else if (Command == "firsthit")
	{
		ray3f const Ray = Scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), Scene->GetParams().WindowSize);
		SRayHitResults const Results = Scene->GetRayHitResults(Scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), Scene->GetParams().WindowSize));
		float const Intersection = Results.T;
		CObject const * const Object = Results.Object;

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << Ray << std::endl;

		if (Object)
		{
			std::cout << "T = " << Intersection << std::endl;
			std::cout << "Object Type: " << Object->GetObjectType() << std::endl;
			std::cout << "Color: " << Object->GetMaterial().Color << std::endl;
		}
		else
		{
			std::cout << "No Hit" << std::endl;
		}
		return;
	}
	else if (Command == "pixelcolor")
	{
		SPixel const Pixel = Scene->CastRaysForPixel(glm::ivec2(X, Y));
		ray3f const Ray = Scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), Scene->GetParams().WindowSize);
		SRayHitResults const Results = Scene->GetRayHitResults(Scene->GetCamera().GetPixelRay(glm::ivec2(X, Y), Scene->GetParams().WindowSize));
		float const Intersection = Results.T;
		CObject const * const Object = Results.Object;

		std::cout << "Pixel: [" << X << ", " << Y << "] Ray: " << Ray << std::endl;

		if (Object)
		{
			std::cout << "T = " << Intersection << std::endl;
			std::cout << "Object Type: " << Object->GetObjectType() << std::endl;
			std::cout << "BRDF: " << "Blinn-Phong" << std::endl;
			std::cout << "Color: " << Pixel << std::endl;
		}
		else
		{
			std::cout << "No Hit" << std::endl;
		}
		return;
	}
	else if (Command == "printrays")
	{
		PrintRayInfo(Scene, X, Y, false);
		return;
	}
	else if (Command == "pixeltrace")
	{
		PrintRayInfo(Scene, X, Y, true);
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
		string const & Arg = CommandArguments[i];
		if (Arg == "-altbrdf")
		{
			Params.UseCookTorrance = true;
		}
		else if (Arg == "-normals")
		{
			Params.DebugNormals = true;
		}
	}
}

void Application::RunCommandSceneInfo()
{
	Scene->SetParams(Params);
	Factory->LoadPovrayScene(FileName);

	CCamera const & Camera = Scene->GetCamera();

	std::cout << "Camera:" << std::endl;
	std::cout << "- Location: {" << Camera.GetPosition() << "}" << std::endl;
	std::cout << "- Up: {" << Camera.GetUpVector() << "}" << std::endl;
	std::cout << "- Right: {" << Camera.GetRightVector() << "}" << std::endl;
	std::cout << "- Look at: {" << Camera.GetLookAt() << "}" << std::endl;
	std::cout << std::endl;
	std::cout << "---" << std::endl;
	std::cout << std::endl;

	std::cout << Scene->GetLights().size() << " light(s)" << std::endl;
	for (size_t i = 0; i < Scene->GetLights().size(); ++ i)
	{
		std::cout << std::endl;
		std::cout << "Light[" << i << "]:" << std::endl;
		std::cout << "- Location: {" << Scene->GetLights()[i]->Position << "}" << std::endl;
		std::cout << "- Color: {" << Scene->GetLights()[i]->Color << "}" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "---" << std::endl;
	std::cout << std::endl;

	std::cout << Scene->GetObjects().size() << " object(s)" << std::endl;
	for (size_t i = 0; i < Scene->GetObjects().size(); ++ i)
	{
		std::cout << std::endl;
		std::cout << "Object[" << i << "]:" << std::endl;
		std::cout << "- Type: " << Scene->GetObjects()[i]->GetObjectType() << std::endl;
		std::cout << Scene->GetObjects()[i]->GetObjectInfo();
	}
}

void Application::DrawSceneThreaded()
{
	bool const UseThreads = true;
	glm::ivec2 const ScreenSize = Params.WindowSize;

	unsigned char * ImageBuffer = new unsigned char[ScreenSize.X * ScreenSize.Y * 4];
	for (int x = 0; x < ScreenSize.X; ++ x)
	{
		for (int y = 0; y < ScreenSize.Y; ++ y)
		{
			ImageBuffer[x * 4 + y * 4 * ScreenSize.X + 0] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.X + 1] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.X + 2] = 0;
			ImageBuffer[x * 4 + y * 4 * ScreenSize.X + 3] = 0;
		}
	}
	ion::CImage * Image = new ion::CImage(ImageBuffer, ScreenSize, 4);


	/////////////////
	// Scene Setup //
	/////////////////

	Scene->SetParams(Params);
	Factory->LoadPovrayScene(FileName);

	if (Params.BoundingVolumeHierarchy)
	{
		Scene->BuildBoundingVolumeHierarchy();
	}


	///////////////////
	// Render Thread //
	///////////////////

	static int const ThreadCount = 12;
	int const PixelCount = ScreenSize.X * ScreenSize.Y;

	std::atomic<int> DoneCount;
	std::atomic<int> CurrentPixel;
	std::atomic<bool> Rendering;

	DoneCount = 0;
	CurrentPixel = 0;
	Rendering = true;

	auto RenderKernel = [&](int const ThreadIndex)
	{
		while (Rendering)
		{
			int Pixel = CurrentPixel ++;

			if (Pixel >= PixelCount)
			{
				break;
			}

			int const X = Pixel / ScreenSize.Y;
			int const Y = Pixel % ScreenSize.Y;

			SPixel p = Scene->CastRaysForPixel(glm::ivec2(X, Y));
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.X + 0] = p.Red;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.X + 1] = p.Green;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.X + 2] = p.Blue;
			ImageBuffer[X * 4 + Y * 4 * ScreenSize.X + 3] = 255;
		}

		DoneCount ++;

		if (ThreadIndex == 0)
		{
			while (DoneCount < ThreadCount)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
#ifdef ION_RAYTRACER_DIAGNOSTICS
			Log::Info("Traced %u rays", Scene->Diagnostics.RayCount);
			Log::Info("Performed %u bounding volume collision checks", Scene->Diagnostics.BoundingVolumeCollision);
			Log::Info("Performed %u sphere collision checks", Scene->Diagnostics.SphereCollision);
#endif
		}
	};

	vector<thread> Threads;
	for (int i = 0; i < ThreadCount; ++ i)
	{
		Threads.push_back(thread(RenderKernel, i));
	}
	for (int i = 0; i < ThreadCount; ++ i)
	{
		Threads[i].join();
	}

	Rendering = false;
	Image->FlipY();

	// Image->Write(File::StripExtension(File) + ".png");
	Image->Write("output.png");
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
