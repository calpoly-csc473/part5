
#include "Renderer.hpp"

#include <atomic>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>


void Renderer::DrawThreaded(RayTracer * rayTracer, Scene * scene)
{
	glm::ivec2 const imageSize = rayTracer->GetParams().imageSize;
	unsigned char * imageBuffer = new unsigned char[imageSize.x * imageSize.y * 4]();

	static int const ThreadCount = 8;
	int const PixelCount = imageSize.x * imageSize.y;

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

			int const X = pixel / imageSize.y;
			int const Y = pixel % imageSize.y;

			Pixel p = rayTracer->CastRaysForPixel(glm::ivec2(X, Y));
			imageBuffer[X * 4 + (imageSize.y - 1 - Y) * 4 * imageSize.x + 0] = p.Red;
			imageBuffer[X * 4 + (imageSize.y - 1 - Y) * 4 * imageSize.x + 1] = p.Green;
			imageBuffer[X * 4 + (imageSize.y - 1 - Y) * 4 * imageSize.x + 2] = p.Blue;
			imageBuffer[X * 4 + (imageSize.y - 1 - Y) * 4 * imageSize.x + 3] = 255;
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

	stbi_write_png("output.png", imageSize.x, imageSize.y, 4, imageBuffer, imageSize.x * 4);
	delete[] imageBuffer;
}
