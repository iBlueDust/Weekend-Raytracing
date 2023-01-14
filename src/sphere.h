#pragma once

#include <memory>
#include <optional>

#include "hittable.h"
#include "vec3.h"


class Sphere : public Hittable {
public:
	point3 center;
	double radius;
	std::shared_ptr<Material> materialPtr;

	Sphere() {}
	Sphere(point3 center, double radius, std::shared_ptr<Material> materialPtr) 
		: center(center), radius(radius), materialPtr(materialPtr) {}

	virtual std::optional<HitRecord> hit
		(const Ray& ray, double tMin, double tMax) const override;

	virtual std::optional<BoundingBox> boundingBox
		(double tStart, double tEnd) const override;
};

std::optional<HitRecord> Sphere::hit(
	const Ray& ray, double tMin, double tMax
) const {
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

	HitRecord result;
	result.intersection = ray.at(t);
	result.t = t;
	auto outwardNormal = (result.intersection - center) / radius;
	result.setNormalFromOutwardNormal(ray, outwardNormal);
	result.materialPtr = materialPtr;

	return result;
}

std::optional<BoundingBox> Sphere::boundingBox(
	double tStart, double tEnd
) const {
	auto halfBox = point3(radius);
	return BoundingBox(center - halfBox, center + halfBox);
}