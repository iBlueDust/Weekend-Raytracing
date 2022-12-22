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

	metal(const color3& albedo) : albedo(albedo) {}

	virtual std::optional<scatter_result> scatter(
		const ray& rayIn, const hit_record& record
	) const override {
		auto rayDirection = rayIn.direction.unit();
		auto normal = record.normal;
		vec3 reflected = rayDirection - 2 * rayDirection.dot(normal) * normal;

		// If ray is coming from inside somehow
		if (reflected.dot(record.normal) <= 0.0)
			return {};

		scatter_result result = {
			/* outRay */      ray(record.intersection, reflected),
			/* attenuation */ albedo
		};
		return std::optional(result);
	}
};