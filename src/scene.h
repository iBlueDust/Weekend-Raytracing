#pragma once

#include <memory>

#include "camera.h"
#include "commons.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "mesh.h"
#include "sphere.h"

class scene {
public:
	scene() {}

	virtual hittable_list build() = 0;
	virtual camera makeCamera(double aspectRatio) = 0;
};


// SCENES

class tutorial_scene : public scene {
public:
	tutorial_scene() {}

	virtual hittable_list build() override {
		hittable_list world;

		auto materialGround = std::make_shared<lambertian_diffuse>(color3(0.8, 0.8, 0.0));
		auto materialCenter = std::make_shared<lambertian_diffuse>(color3(0.1, 0.2, 0.5));
		auto materialLeft = std::make_shared<dielectric>(1.5);
		auto materialRight = std::make_shared<metal>(color3(0.8, 0.6, 0.2), 0.0);


		world.addMany({
			std::make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, materialGround),
			std::make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, materialCenter),
			std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, materialLeft),
			std::make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, materialLeft),
			std::make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, materialRight)
			});
		return world;
	}

	virtual camera makeCamera(double aspectRatio) override {
		camera_config cameraConfig;
		cameraConfig.lookFrom = point3(3, 3, 2);
		cameraConfig.lookAt = point3(0, 0, -1);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 20; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 2.0;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return camera(cameraConfig);
	}
};

class book_cover_scene : public scene {
public:
	book_cover_scene() {}

	virtual hittable_list build() override {
		hittable_list world;

		auto ground_material = std::make_shared<lambertian_diffuse>(color3(0.5, 0.5, 0.5));
		world.add(std::make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto chooseMaterial = globalRng.randomDouble();
				point3 center(
					a + 0.9 * globalRng.randomDouble(),
					0.2,
					b + 0.9 * globalRng.randomDouble()
				);

				if ((center - point3(4, 0.2, 0)).magnitude() > 0.9) {
					std::shared_ptr<material> sphereMaterial;

					if (chooseMaterial < 0.8) {
						// diffuse
						auto albedo = color3::random(globalRng) * color3::random(globalRng);
						sphereMaterial = std::make_shared<lambertian_diffuse>(albedo);
						world.add(std::make_shared<sphere>(center, 0.2, sphereMaterial));
					}
					else if (chooseMaterial < 0.95) {
						// metal
						auto albedo = color3::random(globalRng, 0.5, 1);
						auto fuzz = globalRng.randomDouble(0, 0.5);
						sphereMaterial = std::make_shared<metal>(albedo, fuzz);
						world.add(std::make_shared<sphere>(center, 0.2, sphereMaterial));
					}
					else {
						// glass
						sphereMaterial = std::make_shared<dielectric>(1.5);
						world.add(std::make_shared<sphere>(center, 0.2, sphereMaterial));
					}
				}
			}
		}

		auto material1 = std::make_shared<dielectric>(1.5);
		world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<lambertian_diffuse>(color3(0.4, 0.2, 0.1));
		world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<metal>(color3(0.7, 0.6, 0.5), 0.0);
		world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

		return world;
	}

	virtual camera makeCamera(double aspectRatio) {
		camera_config cameraConfig;
		cameraConfig.lookFrom = point3(13, 2, 3);
		cameraConfig.lookAt = point3(0, 0, 0);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 20; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 0.1;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return camera(cameraConfig);
	}
};

class cornell_box_scene : public scene {
public:
	cornell_box_scene() {}

	virtual hittable_list build() override {
		hittable_list world;

		world.addMany({
			std::make_shared<mesh>(
				std::initializer_list<point3>{
					point3(-1, -1, -2),    // 0
					point3(-1, -1, 1),     // 1
					point3(1, -1, 1),      // 2
					point3(1, -1, -2),     // 3

					point3(-1, 1, -2),     // 4
					point3(-1, 1, 1),      // 5
					point3(1, 1, -2),      // 6
					point3(1, 1, 1),       // 7

					point3(-0.5, 1, -0.5), // 8
					point3(-0.5, 1, 0.5),  // 9
					point3(0.5, 1, -0.5),  // 10
					point3(0.5, 1, 0.5)    // 11
				},
				std::initializer_list<int>{
					0, 1, 2, 0, 2, 3, // bottom face
					2, 3, 7, 3, 7, 6, // left face
					5, 4, 1, 1, 0, 4, // right face
					1, 2, 7, 1, 7, 5, // back face
					0, 3, 6, 0, 6, 4, // front face (behind camera)

					// top face
					7, 5, 11, 11, 5, 9,  // front eave
					10, 6, 7, 10, 7, 11, // left eave
					5, 4, 9, 9, 4, 8,    // right eave
					4, 6, 8, 8, 6, 10    // back eave 
				},
				std::initializer_list<std::shared_ptr<material>> {
					std::make_shared<lambertian_diffuse>(color3(1)),
					std::make_shared<lambertian_diffuse>(color3(1, 0, 0)),
					std::make_shared<lambertian_diffuse>(color3(0, 1, 0))
					},
				std::initializer_list<int> {
					0, 0, // bottom face
					1, 1, // left face
					2, 2, // right face
					0 // everything else
				}
			),
			std::make_shared<sphere>(
				point3(-0.5, -0.65, 0.1),
				0.35,
				std::make_shared<metal>(color3(0, 0.2, 0.8), 0.8)
			),
			std::make_shared<sphere>(
				point3(0.4, -0.5, 0.3),
				0.5,
				std::make_shared<lambertian_diffuse>(color3(0.4, 0.1, 0))
			),
			std::make_shared<sphere>(
				point3(0, -0.6, -0.2),
				0.4,
				std::make_shared<dielectric>(1.250)
			),
		});

		return world;
	}

	virtual camera makeCamera(double aspectRatio) {
		camera_config cameraConfig;
		cameraConfig.lookFrom = point3(0, 0, -1.95);
		cameraConfig.lookAt = point3(0, 0, 1);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 85; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 0.05;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return camera(cameraConfig);
	}
};
