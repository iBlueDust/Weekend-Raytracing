// Structs and classes for all things in a scene that will
// cause a ray to change or resolve.

#pragma once

#include <memory>
#include <optional>

#include "ray.h"

class Material;

struct HitRecord {
	point3 intersection;
	vec3 normal;
	std::shared_ptr<Material> materialPtr;
	double t; // parameter of ray
	bool frontFace; // whether the normal faces in this direction (& for back-face culling)

	// Outward normal refers to the normal that may not necessarily point out of a hittable object
	inline void setNormalFromOutwardNormal(const Ray& ray, const vec3& outwardNormal) {
		// the ray and the normal should be facing against each other
		frontFace = ray.direction.dot(outwardNormal) < 0.0; 
		normal = frontFace ? outwardNormal : -outwardNormal;
	}
};

class Hittable {
public:
	virtual std::optional<HitRecord> hit
	(const Ray& ray, double tMin, double tMax) const = 0;

};