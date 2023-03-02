#pragma once

#include <memory>
#include <optional>

#include "bounding_box.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include "vec3.h"

#define EPSILON 0.00001

class Triangle : public Hittable {
public:
	const point3 a, b, c;
	const point3 normal;
	std::shared_ptr<Material> materialPtr;

	Triangle() {}
	Triangle(
		const point3& a, const point3& b, const point3& c,
		const std::shared_ptr<Material>& materialPtr
	) : 
		a(a), b(b), c(c), 
		normal((b - a).cross(c - a).unit()), 
		materialPtr(materialPtr) {}

	virtual std::optional<HitRecord> hit(
		const Ray& ray, double tMin, double tMax
	) const {
		double t = hitPlane(ray);
		if (t < tMin || t > tMax)
			return {};

		auto intersection = ray.at(t);
		if (!includesPointOnPlane(intersection))
			return {};

		HitRecord record;
		record.t = t;
		record.intersection = intersection;
		record.materialPtr = materialPtr;
		record.setNormalFromOutwardNormal(ray, normal);
		return record;
	}

	virtual std::optional<BoundingBox> boundingBox(
		double tStart, double tEnd
	) const {
		return BoundingBox(
			point3(
				std::min<double>({ a.x, b.x, c.x }) - EPSILON,
				std::min<double>({ a.y, b.y, c.y }) - EPSILON,
				std::min<double>({ a.z, b.z, c.z }) - EPSILON
			),
			point3(
				std::max<double>({ a.x, b.x, c.x }) + EPSILON,
				std::max<double>({ a.y, b.y, c.y }) + EPSILON,
				std::max<double>({ a.z, b.z, c.z }) + EPSILON
			)
		);
	}

private:
	// Returns a value t where ray.at(t) is the intersection of the plane 
		// and the ray
	double hitPlane(const Ray& ray) const {
		double D = a.dot(normal); // From the plane equation Ax + By + Cz = D
		double t = (D - normal.dot(ray.origin)) / normal.dot(ray.direction);
		return t;
	}

	// Checks if a 3D point, assumed to be on the plane of the triangle,
		// is inside that triangle
	bool includesPointOnPlane(point3 point) const {
		auto AB = b - a;
		auto BC = c - b;
		auto CA = a - c;

		// Seems to need a floating point buffer region for edgecases
		// where point is right on AB, BC, or CA
		bool rightOfAB = std::signbit(AB.cross(normal).dot(point - a));
		bool rightOfBC = std::signbit(BC.cross(normal).dot(point - b));
		bool rightOfCA = std::signbit(CA.cross(normal).dot(point - c));

		int positiveCount = rightOfAB + rightOfBC + rightOfCA;

		return positiveCount == 0 || positiveCount == 3;
	}
};