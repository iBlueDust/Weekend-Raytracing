// main.cpp : Defines the entry point for the application.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <optional>

#include "main.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"


typedef struct {
	point3 center;
	double radius;
} sphere;


std::optional<point3> hitSphere(const sphere& sphere, const ray& ray) {
	auto deltaCenter = ray.origin - sphere.center;

	// Setup quadratic equation
	double a = ray.direction.squareMagnitude();
	double b = 2 * ray.direction.dot(deltaCenter);
	double c = deltaCenter.squareMagnitude() - sphere.radius * sphere.radius;

	auto discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0)
		return {};

	double sqrtDiscriminant = sqrt(discriminant);
	double t1 = (-b + sqrtDiscriminant) / (2 * a);
	double t2 = (-b - sqrtDiscriminant) / (2 * a);
	double t;

	// If ray starts from inside the circle
	// (notice that t1 >= t2)
	if (t2 < 0) {
		t = t1;
	} else {
		t = t2;
	}

	return ray.at(t);
}


color3 rayColor(const ray& ray) {
	const sphere sphere = { { 0, 0, -1 }, 0.5 };
	auto hitPoint = hitSphere(sphere, ray);
	if (hitPoint.has_value()) {
		auto normal = (hitPoint.value() - sphere.center).unit();
		return static_cast<color3>(0.5 * (normal + 1.0));
	}

	vec3 unitDirection = ray.direction.unit();
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

	// Camera
	double viewportHeight = 2.0;
	double viewportWidth = aspectRatio * viewportHeight;
	double focalLength = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewportWidth, 0, 0);
	auto vertical = vec3(0, viewportHeight, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focalLength);

	// Render

	imageFile << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

	for (int j = imageHeight - 1; j >= 0; j--) {
		printf("\rScanlines remaining: %d ", j);
		fflush(stdout);
		for (int i = 0; i < imageWidth; i++) {
			auto u = double(i) / (imageWidth - 1);
			auto v = double(j) / (imageHeight - 1);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			color3 pixelColor = rayColor(r);

			writePixel(imageFile, pixelColor);
		}
	}

	printf("\nDone.\n");

	imageFile.close();
	return 0;
}
