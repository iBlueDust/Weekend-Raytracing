#pragma once

#include <memory>

#include "camera.h"
#include "commons.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "mesh.h"
#include "sphere.h"

class Scene {
public:
	Scene() {}

	virtual HittableList build() = 0;
	virtual Camera makeCamera(double aspectRatio) = 0;
};


// SCENES

class TutorialScene : public Scene {
public:
	TutorialScene() {}

	virtual HittableList build() override {
		HittableList world;

		auto materialGround = std::make_shared<LambertianDiffuse>(color3(0.8, 0.8, 0.0));
		auto materialCenter = std::make_shared<LambertianDiffuse>(color3(0.1, 0.2, 0.5));
		auto materialLeft = std::make_shared<Dielectric>(1.5);
		auto materialRight = std::make_shared<Metal>(color3(0.8, 0.6, 0.2), 0.0);


		world.addMany({
			std::make_shared<Sphere>(point3(0.0, -100.5, -1.0), 100.0, materialGround),
			std::make_shared<Sphere>(point3(0.0, 0.0, -1.0), 0.5, materialCenter),
			std::make_shared<Sphere>(point3(-1.0, 0.0, -1.0), 0.5, materialLeft),
			std::make_shared<Sphere>(point3(-1.0, 0.0, -1.0), -0.45, materialLeft),
			std::make_shared<Sphere>(point3(1.0, 0.0, -1.0), 0.5, materialRight)
			});
		return world;
	}

	virtual Camera makeCamera(double aspectRatio) override {
		CameraConfig cameraConfig;
		cameraConfig.lookFrom = point3(3, 3, 2);
		cameraConfig.lookAt = point3(0, 0, -1);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 20; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 2.0;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return Camera(cameraConfig);
	}
};

class BookCoverScene : public Scene {
public:
	BookCoverScene() {}

	virtual HittableList build() override {
		HittableList world;

		auto ground_material = std::make_shared<LambertianDiffuse>(color3(0.5, 0.5, 0.5));
		world.add(std::make_shared<Sphere>(point3(0, -1000, 0), 1000, ground_material));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto chooseMaterial = globalRng.randomDouble();
				point3 center(
					a + 0.9 * globalRng.randomDouble(),
					0.2,
					b + 0.9 * globalRng.randomDouble()
				);

				if ((center - point3(4, 0.2, 0)).magnitude() > 0.9) {
					std::shared_ptr<Material> sphereMaterial;

					if (chooseMaterial < 0.8) {
						// diffuse
						auto albedo = color3::random(globalRng) * color3::random(globalRng);
						sphereMaterial = std::make_shared<LambertianDiffuse>(albedo);
						world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
					else if (chooseMaterial < 0.95) {
						// metal
						auto albedo = color3::random(globalRng, 0.5, 1);
						auto fuzz = globalRng.randomDouble(0, 0.5);
						sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
						world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
					else {
						// glass
						sphereMaterial = std::make_shared<Dielectric>(1.5);
						world.add(std::make_shared<Sphere>(center, 0.2, sphereMaterial));
					}
				}
			}
		}

		auto material1 = std::make_shared<Dielectric>(1.5);
		world.add(std::make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<LambertianDiffuse>(color3(0.4, 0.2, 0.1));
		world.add(std::make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<Metal>(color3(0.7, 0.6, 0.5), 0.0);
		world.add(std::make_shared<Sphere>(point3(4, 1, 0), 1.0, material3));

		return world;
	}

	virtual Camera makeCamera(double aspectRatio) {
		CameraConfig cameraConfig;
		cameraConfig.lookFrom = point3(13, 2, 3);
		cameraConfig.lookAt = point3(0, 0, 0);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 20; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 0.1;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return Camera(cameraConfig);
	}
};

class CornellBoxScene : public Scene {
public:
	CornellBoxScene() {}

	virtual HittableList build() override {
		HittableList world;

		world.addMany({
			std::make_shared<Mesh>(
				std::initializer_list<point3>{
					point3(-1, -1, -2),      // 0
					point3(-1, -1, 1),       // 1
					point3(1, -1, 1),        // 2
					point3(1, -1, -2),       // 3

					point3(-1, 1, -2),       // 4
					point3(-1, 1, 1),        // 5
					point3(1, 1, 1),         // 6
					point3(1, 1, -2),        // 7

					point3(-0.25, 1, -0.25), // 8
					point3(-0.25, 1, 0.25),  // 9
					point3(0.25, 1, 0.25),   // 10
					point3(0.25, 1, -0.25)   // 11
				},
				std::initializer_list<int>{
					2, 1, 0, 3, 2, 0, // bottom face
					1, 5, 4, 0, 1, 4, // left face
					2, 3, 6, 7, 6, 3, // right face
					6, 5, 1, 6, 1, 2, // back face
					4, 3, 0, 7, 3, 4, // front face (behind camera)

					// top face
					8, 9, 10, 10, 11, 8, // center panel
					8, 7, 4, 7, 8, 11,   // front eave
					4, 5, 9, 9, 8, 4,    // left eave
					6, 7, 11, 6, 11, 10, // right eave
					5, 6, 10, 10, 9, 5   // back eave 
				},
				std::initializer_list<std::shared_ptr<Material>> {
					std::make_shared<LambertianDiffuse>(color3(0.73)),
					std::make_shared<LambertianDiffuse>(color3(1, 0, 0)),
					std::make_shared<LambertianDiffuse>(color3(0, 1, 0)),
					std::make_shared<DiffuseLight>(color3(1) * 15.0)
				},
				std::initializer_list<int> {
					0, 0, // bottom face
					1, 1, // left face
					2, 2, // right face
					0, 0, // back face
					0, 0, // front face
					
					// top face
					3, 3, // center panel
					0 // everything else
				}
			),

			std::make_shared<Sphere>(
				point3(-0.5, -0.65, 0.1),
				0.35,
				std::make_shared<Metal>(color3(0, 0.2, 0.8), 0.8)
			),
			std::make_shared<Sphere>(
				point3(0.4, -0.5, 0.3),
				0.5,
				std::make_shared<LambertianDiffuse>(color3(0.4, 0.1, 0))
			),
			std::make_shared<Sphere>(
				point3(0, -0.6, -0.2),
				0.4,
				std::make_shared<Dielectric>(1.250)
			)
		});

		return world;
	}

	virtual Camera makeCamera(double aspectRatio) {
		CameraConfig cameraConfig;
		cameraConfig.lookFrom = point3(0, 0, -1.95);
		cameraConfig.lookAt = point3(0, 0, 0);
		cameraConfig.worldUp = vec3(0, 1, 0);
		cameraConfig.verticalFovInDegrees = 85; // in degrees
		cameraConfig.aspectRatio = aspectRatio;
		cameraConfig.aperture = 0.05;
		cameraConfig.focalLength =
			(cameraConfig.lookAt - cameraConfig.lookFrom).magnitude();

		return Camera(cameraConfig);
	}
};
