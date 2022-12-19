#pragma once

#include "vec3.h"

class ray {
public:
	// Using linear algebra representation
	// r = a + bt
	point3 origin;
	vec3 direction; // author explicitly chose against making this a unit vector

	ray() {}
	ray(const point3& origin, const vec3& direction)
		: origin(origin), direction(direction) {}

	point3 at(double t) const {
		return origin + t * direction;
	}
};