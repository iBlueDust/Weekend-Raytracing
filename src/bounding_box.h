#pragma once

#include <cassert>

#include "ray.h"
#include "vec3.h"

// Axis-Aligned Bounding Box (AABB)
class BoundingBox {
public:
	BoundingBox() : cornerMin(0), cornerMax(0) {}
	BoundingBox(point3 cornerMin, point3 cornerMax) :
		cornerMin(cornerMin), cornerMax(cornerMax) {
		
		assert(
			cornerMin.x <= cornerMax.x
			&& cornerMin.y <= cornerMax.y
			&& cornerMin.z <= cornerMax.z
		);
	}

	bool contains(const point3& point) const {
		return cornerMin.x <= point.x && cornerMax.x >= point.x
			&& cornerMin.y <= point.y && cornerMax.y >= point.y
			&& cornerMin.z <= point.z && cornerMax.z >= point.z;
	}

	// https://raytracing.github.io/books/RayTracingTheNextWeek#boundingvolumehierarchies/anoptimizedaabbhitmethod
	bool hit(const Ray& ray, double tMin, double tMax) const {
		for (int dimension = 0; dimension < 3; dimension++) {
			double inverseDirection = 1.0 / ray.direction[dimension];
			double t0 = (cornerMin[dimension] - ray.origin[dimension])
				* inverseDirection;
			double t1 = (cornerMax[dimension] - ray.origin[dimension])
				* inverseDirection;

			if (inverseDirection < 0.0)
				std::swap(t0, t1);

			tMin = std::max<double>(tMin, t0);
			tMax = std::min<double>(tMax, t1);

			if (tMax <= tMin)
				return false;
		}

		return true;
	}

	void include(const point3& point) {
		cornerMin.x = std::min<double>(cornerMin.x, point.x);
		cornerMin.y = std::min<double>(cornerMin.y, point.y);
		cornerMin.z = std::min<double>(cornerMin.z, point.z);

		cornerMax.x = std::max<double>(cornerMax.x, point.x);
		cornerMax.y = std::max<double>(cornerMax.y, point.y);
		cornerMax.z = std::max<double>(cornerMax.z, point.z);
	}

	static BoundingBox merge(const BoundingBox& a, const BoundingBox& b) {
		return BoundingBox(
			point3(
				std::min<double>(a.cornerMin.x, b.cornerMin.x),
				std::min<double>(a.cornerMin.y, b.cornerMin.y),
				std::min<double>(a.cornerMin.z, b.cornerMin.z)
			),
			point3(
				std::max<double>(a.cornerMax.x, b.cornerMax.x),
				std::max<double>(a.cornerMax.y, b.cornerMax.y),
				std::max<double>(a.cornerMax.z, b.cornerMax.z)
			)
		);
	}

	// It is always true that
	// cornerMin.x <= cornerMax.x
	// cornerMin.y <= cornerMax.y
	// cornerMin.z <= cornerMax.z
	point3 cornerMin;
	point3 cornerMax;
};

