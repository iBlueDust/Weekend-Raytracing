#pragma once

#include <memory>
#include <optional>

#include "hittable.h"
#include "vec3.h"


class sphere : public hittable {
public:
	point3 center;
	double radius;
	std::shared_ptr<material> materialPtr;

	sphere() {}
	sphere(point3 center, double radius, std::shared_ptr<material> materialPtr) 
		: center(center), radius(radius), materialPtr(materialPtr) {}

	virtual std::optional<hit_record> hit(const ray& ray, double tMin, double tMax) const override;
};

std::optional<hit_record> sphere::hit(const ray& ray, double tMin, double tMax) const {
	auto deltaCenter = ray.origin - center;

	// Setup quadratic equation
	double a = ray.direction.squareMagnitude();
	double b = 2 * ray.direction.dot(deltaCenter);
	double c = deltaCenter.squareMagnitude() - radius * radius;

	auto discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0)
		return {};

	// Find root between tMin and tMax
	double sqrtDiscriminant = sqrt(discriminant);
	double t = (-b - sqrtDiscriminant) / (2 * a);
	if (t < tMin || tMax < t) {
		t += sqrtDiscriminant / a;
		if (t < tMin || tMax < t) {
			return {};
		}
	}	

	hit_record result;
	result.intersection = ray.at(t);
	result.t = t;
	auto outwardNormal = (result.intersection - center) / radius;
	result.setNormalFromOutwardNormal(ray, outwardNormal);
	result.materialPtr = materialPtr;

	return result;
}