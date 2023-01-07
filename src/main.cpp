// main.cpp : Defines the entry point for the application.
//

#include <fstream>
#include <iostream>
#include <memory>
#include <minmax.h>
#include <numbers>
#include <numeric>
#include <optional>
#include <stdio.h>
#include <thread>

#include "main.h"
#include "camera.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"


color3 rayColor(
	const HittableList& world, 
	const color3& background,
	const Ray& ray, 
	const int maxBounces, 
	RandomNumberGenerator& rng
) {
	constexpr double INFTY = std::numeric_limits<double>::infinity();
	constexpr double absorption = 0.5;

	if (maxBounces <= 0)
		return color3(0);

	// 0.001 comes from "8.4 Fixing Shadow Acne"
	auto hit = world.hit(ray, 0.001, INFTY);
	if (!hit)
		return background;

	auto record = hit.value();
	auto scattered = record.materialPtr->scatter(ray, record, rng);
	color3 emitted = record.materialPtr->emit();

	if (!scattered)
		return emitted;

	auto scatterResult = scattered.value();
	return emitted + 
		scatterResult.attenuation
		* rayColor(world, background, scatterResult.outRay, maxBounces - 1, rng);
}

void render(
	const int width,
	const int height,
	const int sampleCount,
	const int seed,
	const int maxBounces,
	const HittableList& world,
	const Camera& camera,
	std::vector<color3>& image,
	int& scanlinesDone
) {
	RandomNumberGenerator rng(seed);
	scanlinesDone = 0;

	// Origin is at the bottom left corner
	for (int j = 0; j < height; j++) {

		for (int i = 0; i < width; i++) {

			color3 pixel(0, 0, 0);
			for (int s = 0; s < sampleCount; s++) {
				auto column = i;
				auto row = height - j - 1;
				auto u = double(column + rng.randomDouble()) / (width - 1);
				auto v = double(row + rng.randomDouble()) / (height - 1);

				Ray ray = camera.rayFromUV(u, v, rng);
				pixel += rayColor(world, color3(0), ray, maxBounces, rng);
			}
			image[j * width + i] = pixel / sampleCount;
		}

		scanlinesDone++;
	}
}


int main(int argc, char** argv) {

	if (argc != 2) {
		printf(
			"Please specify an output file.\n"
			"For example,\n"
			"	WeekendRaytracing.exe output.ppm\n"
		);
		return 1;
	}

	// File
	std::ofstream imageFile(argv[1]);
	if (!imageFile.is_open()) {
		// Check this line for vulnerabilities vvv
		printf("Error opening file %.200s\n", argv[1]);
		return 1;
	}

	// Image

	const double aspectRatio = 1.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int sampleCount = 100;
	const int maxBounces = 50;

	// World
	cornell_box_scene masterScene;
	HittableList world = masterScene.build();

	// Camera
	Camera mainCamera = masterScene.makeCamera(aspectRatio);

	// Render
#ifdef NDEBUG
	const unsigned int threadCount = std::max<unsigned int>(std::thread::hardware_concurrency(), 1);
#else
	const unsigned int threadCount = 1u;
#endif

	std::vector<std::thread> threads;
	std::vector<int> scanlinesDoneByThread(threadCount, 0);
	std::vector<std::vector<color3>> imagesByThread;
	threads.reserve(threadCount);
	imagesByThread.reserve(threadCount);

	auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
	).count();

	// To make sure all samples are rendered although threadCount doesn't 
	// divide sampleCount.
	int samplesLeftToAllocate = sampleCount;

	for (int i = 0; i < threadCount; i++) {
		int threadSampleCount = samplesLeftToAllocate / (threadCount - i);
		samplesLeftToAllocate -= threadSampleCount;
		
		imagesByThread.push_back(
			std::vector<color3>(imageWidth * imageHeight, color3(0))
		);
		threads.push_back(
			std::thread(
				render,
				// args
				imageWidth,
				imageHeight,
				threadSampleCount,
				seed,
				maxBounces,
				std::ref(world),
				std::ref(mainCamera),
				std::ref(imagesByThread[i]),
				std::ref(scanlinesDoneByThread[i])
			)
		);
		
		seed++;
	}

	// Progress Reporting

	while (
		std::none_of(
			scanlinesDoneByThread.begin(),
			scanlinesDoneByThread.end(),
			[=](int scanlinesDone) { return scanlinesDone >= imageHeight; }
		)
	) {
		int totalScanlinesDone = std::accumulate(
			scanlinesDoneByThread.begin(),
			scanlinesDoneByThread.end(),
			0 // initial value
		);

		int weightedScanlinesDone = totalScanlinesDone / threadCount;
		double progress = 100.0 * totalScanlinesDone / (imageHeight * threadCount);

		printf(
			"\rRendering on %d thread(s): %5d/%5d scanlines done (%.2f%%)",
			threadCount,
			weightedScanlinesDone,
			imageHeight,
			progress
		);
		fflush(stdin);

		// Wait for 1000ms
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	printf(
		"\rRendering on %d thread(s): %5d/%5d scanlines done (100.00%%)\n",
		threadCount,
		imageHeight,
		imageHeight
	);

	// Join threads

	for (auto& thread : threads) {
		thread.join();
	}

	// Saving

	printf("Saving...\n");

	imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int j = 0; j < imageHeight; j++) {
		for (int i = 0; i < imageWidth; i++) {
			color3 pixel;
			for (auto& threadImage : imagesByThread) {
				pixel += threadImage.at(j * imageWidth + i);
			}
			pixel /= threadCount;

			writePixel(imageFile, gammaCorrect(pixel));
		}
	}

	printf("Done.\n");

	imageFile.close();
	return 0;
}
