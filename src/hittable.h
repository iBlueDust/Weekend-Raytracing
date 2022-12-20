// Structs and classes for all things in a scene that will
// cause a ray to change or resolve.

#pragma once

#include <optional>

#include "ray.h"

struct hit_record {
	point3 intersection;
	vec3 normal;
	double t; // parameter of ray
	bool frontFace; // whether the normal faces in this direction (& for back-face culling)

	// Outward normal refers to the normal that may not necessarily point out of a hittable object
	inline void setNormalFromOutwardNormal(const ray& ray, const vec3& outwardNormal) {
		// the ray and the normal should be facing against each other
		frontFace = ray.direction.dot(outwardNormal) < 0.0; 
		normal = (!frontFace * -1) * outwardNormal;
	}
};

class hittable {
public:
	virtual std::optional<hit_record> hit(const ray& ray, double tMin, double tMax) const = 0;

};