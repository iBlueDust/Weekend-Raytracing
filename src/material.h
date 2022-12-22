#pragma once

#include <optional>

#include "hittable.h"
#include "ray.h"
#include "vec3.h"

struct scatter_result {
	ray outRay;
	color3 attenuation;
};

struct hit_record;

class material {
public:
	virtual std::optional<scatter_result> scatter(
		const ray& rayIn, const hit_record& record
	) const = 0;
};

class lambertian_diffuse : public material {
public:
	color3 albedo;

	lambertian_diffuse(const color3& albedo) : albedo(albedo) {}

	virtual std::optional<scatter_result> scatter(
		const ray& rayIn, const hit_record& record
	) const override {
		auto scatterDirection = record.normal + vec3::randomOnUnitSphere();

		// In case the random vector is almost equal to the opposite of the normal,
		// scatterDirection can become a zero vector, which is invalid
		if (scatterDirection.nearZero())
			scatterDirection = record.normal;

		scatter_result result = {
			/* outRay */      ray(record.intersection, scatterDirection),
			/* attenuation */ albedo
		};
		return std::optional(result);
	}
};

class metal : public material {
public:
	color3 albedo;
	double fuzz;

	metal(const color3& albedo, double fuzz)
		: albedo(albedo), fuzz(std::clamp(fuzz, 0.0, 1.0)) {}

	virtual std::optional<scatter_result> scatter(
		const ray& rayIn, const hit_record& record
	) const override {
		auto inUnitDirection = rayIn.direction.unit();
		auto normal = record.normal;
		vec3 reflected = inUnitDirection.reflect(normal);

		// If ray is coming from inside somehow
		if (reflected.dot(record.normal) <= 0.0)
			return {};

		auto outDirection = reflected + fuzz * vec3::randomInUnitSphere();
		auto outRay = ray(record.intersection, outDirection);

		scatter_result result = {
			/* outRay */      outRay,
			/* attenuation */ albedo
		};
		return std::optional(result);
	}
};

// Material that always refracts light
class dielectric : public material {
public:
	double ior;

	dielectric(double indexOfRefraction) : ior(indexOfRefraction) {}

	virtual std::optional<scatter_result> scatter(
		const ray& rayIn, const hit_record& record
	) const override {
		// Assumes air has an IOR of 1.000
		double iorRatio = record.frontFace ? (1.0 / ior) : ior;
		auto inUnitDirection = rayIn.direction.unit();
		double cosTheta = std::min(-inUnitDirection.dot(record.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		
		vec3 outDirection;

		// Check for total internal reflection
		if (iorRatio * sinTheta > 1.0)
			outDirection = inUnitDirection.reflect(record.normal);
		else
			outDirection = inUnitDirection.refract(record.normal, iorRatio);

		scatter_result result = {
			/* outRay */      ray(record.intersection, outDirection),
			/* attenuation */ color3(1.0) // Always white
		};
		return std::optional(result);
	}
};
