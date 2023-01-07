#pragma once

#include "vec3.h"

class Ray {
public:
	// Using linear algebra representation
	// r = a + bt
	point3 origin;
	vec3 direction; // author explicitly chose against making this a unit vector

	Ray() {}
	Ray(const point3& origin, const vec3& direction)
		: origin(origin), direction(direction) {}

	point3 at(double t) const {
		return origin + direction * t;
	}
};