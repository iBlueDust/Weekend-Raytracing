#pragma once

#include <cmath>
#include <optional>

#include "hittable.h"
#include "ray.h"
#include "rng.h"
#include "vec3.h"

struct ScatterResult {
	Ray outRay;
	color3 attenuation;
};

struct HitRecord;

class Material {
public:
	virtual std::optional<ScatterResult> scatter(
		const Ray& rayIn, 
		const HitRecord& record,
		RandomNumberGenerator& rng
	) const = 0;
};

class LambertianDiffuse : public Material {
public:
	color3 albedo;

	LambertianDiffuse(const color3& albedo) : albedo(albedo) {}

	virtual std::optional<ScatterResult> scatter(
		const Ray& rayIn, 
		const HitRecord& record,
		RandomNumberGenerator& rng
		) const override {
		auto scatterDirection = record.normal + vec3::randomOnUnitSphere(rng);

		// In case the random vector is almost equal to the opposite of the normal,
		// scatterDirection can become a zero vector, which is invalid
		if (scatterDirection.nearZero())
			scatterDirection = record.normal;

		ScatterResult result = {
			/* outRay */      Ray(record.intersection, scatterDirection),
			/* attenuation */ albedo
		};
		return std::optional(result);
	}
};

class Metal : public Material {
public:
	color3 albedo;
	double fuzz;

	Metal(const color3& albedo, double fuzz)
		: albedo(albedo), fuzz(std::clamp(fuzz, 0.0, 1.0)) {}

	virtual std::optional<ScatterResult> scatter(
		const Ray& rayIn, 
		const HitRecord& record,
		RandomNumberGenerator& rng
	) const override {
		auto inUnitDirection = rayIn.direction.unit();
		auto normal = record.normal;
		vec3 reflected = inUnitDirection.reflect(normal);

		// If ray is coming from inside somehow
		if (reflected.dot(record.normal) <= 0.0)
			return {};

		auto outDirection = reflected + fuzz * vec3::randomInUnitSphere(rng);
		auto outRay = Ray(record.intersection, outDirection);

		ScatterResult result = {
			/* outRay */      outRay,
			/* attenuation */ albedo
		};
		return std::optional(result);
	}
};

// Material that always refracts light
class Dielectric : public Material {
public:
	double ior;

	Dielectric(double indexOfRefraction) : ior(indexOfRefraction) {}

	virtual std::optional<ScatterResult> scatter(
		const Ray& rayIn, 
		const HitRecord& record, 
		RandomNumberGenerator& rng
	) const override {
		// Assumes air has an IOR of 1.000
		double iorRatio = record.frontFace ? (1.0 / ior) : ior;
		auto inUnitDirection = rayIn.direction.unit();
		double cosTheta = std::min<double>(-inUnitDirection.dot(record.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		
		bool cantRefract = iorRatio * sinTheta > 1.0;
		vec3 outDirection;

		// Check for total internal reflection
		if (cantRefract || reflectance(cosTheta, iorRatio) > rng.randomDouble())
			outDirection = inUnitDirection.reflect(record.normal);
		else
			outDirection = inUnitDirection.refract(record.normal, iorRatio);

		ScatterResult result = {
			/* outRay */      Ray(record.intersection, outDirection),
			/* attenuation */ color3(1.0) // Always white
		};
		return std::optional(result);
	}

private:
	static double reflectance(double cosTheta, double iorRatio) {
		// Schlick's approximation for fresnel reflectance
		auto r0 = (1.0 - iorRatio) / (1.0 + iorRatio);
		r0 *= r0; // square it
		return r0 + (1.0 - r0) * std::pow(1 - cosTheta, 5);
	}
};
