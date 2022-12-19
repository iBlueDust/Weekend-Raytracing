// main.cpp : Defines the entry point for the application.
//

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "main.h"
#include "vec3.h"


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
	std::ofstream imageFile (argv[1]);
	if (!imageFile.is_open()) {
		// Check this line for vulnerabilities vvv
		printf("Error opening file %.200s\n", argv[1]);
		return 1;
	}
	
	// Image

	const int imageWidth = 256;
	const int imageHeight = 256;

	// Render

	imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int j = imageHeight - 1; j >= 0; j--) {
		printf("\rScanlines remaining: %d ", j);
		fflush(stdout);
		for (int i = 0; i < imageWidth; i++) {
			vec3 color;
			color += vec3(1 / (double(imageWidth) - 1), 1 / (double(imageHeight) - 1), 0.0);
			color *= vec3(double(i), double(j), 0.0);
			color = color + vec3(0.0, 0.0, 0.25);

			auto r = i / (double(imageWidth) - 1);
			auto g = j / (double(imageHeight) - 1);
			auto b = 0.25;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			imageFile << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}

	printf("\nDone.\n");
	
	imageFile.close();
	return 0;
}
