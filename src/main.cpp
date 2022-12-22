// main.cpp : Defines the entry point for the application.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <optional>

#include "main.h"
#include "vec3.h"
#include "camera.h"
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable.h"
#include "hittable_list.h"


color3 rayColor(const hittable_list& world, const ray& r, int maxBounces) {
	constexpr double INFTY = std::numeric_limits<double>::infinity();
	constexpr double absorption = 0.5;

	if (maxBounces <= 0)
		return color3(0);
	
	// 0.001 comes from "8.4 Fixing Shadow Acne"
	auto hit = world.hit(r, 0.001, INFTY);
	if (hit.has_value()) {
		auto record = hit.value();
		vec3 diffuseDirection = record.normal + vec3::randomOnUnitSphere();
		ray diffuseRay(record.intersection, diffuseDirection);
		return absorption * rayColor(world, diffuseRay, maxBounces - 1);
	}

	vec3 unitDirection = r.direction.unit();
	auto t = 0.5 * (unitDirection.y + 1.0);
	return vec3::lerp(color3(1.0), color3(0.5, 0.7, 1.0), t);
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

	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
	const int sampleCount = 100;
	const int maxBounces = 50;

	// World

	hittable_list world;
	world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(point3(0, -100.5, -1), 100));

	// Camera
	
	camera mainCamera;

	// Render

	imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int j = imageHeight - 1; j >= 0; j--) {
		printf("\rScanlines remaining: %d ", j);
		fflush(stdout);
		for (int i = 0; i < imageWidth; i++) {

			color3 pixel(0, 0, 0);
			for (int s = 0; s < sampleCount; s++) {
				auto u = double(i + randomDouble()) / (imageWidth - 1);
				auto v = double(j + randomDouble()) / (imageHeight - 1);

				ray ray = mainCamera.rayFromUV(u, v);
				pixel += rayColor(world, ray, maxBounces);
			}

			writePixel(imageFile, pixel, sampleCount);
		}
	}

	printf("\nDone.\n");

	imageFile.close();
	return 0;
}
