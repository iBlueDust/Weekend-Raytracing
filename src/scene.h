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

class nvidia_scene : public scene {
public:
	nvidia_scene() {}

	virtual hittable_list build() override {
		hittable_list world;

		world.add(
			std::make_shared<mesh>(
				std::initializer_list<point3>{
					point3(0, 1, 0),
					point3(-1, 0, 0),
					point3(1, 0, 0),
					point3(0, -1, 0)
				},
				std::initializer_list<int>{ 0, 1, 2, 1, 3, 2 },
				std::make_shared<metal>(color3(1, 0.8, 0.8), 0.5)
			)
		);

		return world;
	}

	virtual camera makeCamera(double aspectRatio) {
		camera_config cameraConfig;
		cameraConfig.lookFrom = point3(0, 0, -3);
		cameraConfig.lookAt = point3(0, 0, 0);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 90; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 0.1;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return camera(cameraConfig);
	}
};
