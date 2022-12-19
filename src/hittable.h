// Structs and classes for all things in a scene that will
// cause a ray to change or resolve.

#pragma once

#include <optional>

#include "ray.h"

struct hit_record {
	point3 intersection;
	vec3 normal;
	double t; // parameter of ray
};

class hittable {
public:
	virtual std::optional<hit_record> hit(const ray& ray, double tMin, double tMax) const = 0;

};