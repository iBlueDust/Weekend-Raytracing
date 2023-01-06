#pragma once

#include <cmath>
#include <memory>
#include <ranges>
#include <vector>

#include "bounding_box.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

struct triangle {
	point3 a, b, c;
};

class mesh : public hittable {
private:
	// Triangle in 3D space
	class triangle {
	public:
		const point3 a, b, c;
		const vec3 normal;

		triangle(point3 a, point3 b, point3 c) 
			: a(a), b(b), c(c), normal((b - a).cross(c - a)) { }

		// Returns a value t where ray.at(t) is the intersection of the plane 
		// and the ray
		double hitPlane(const ray& ray) const {
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

			bool rightOfAB = std::signbit(AB.cross(normal).dot(point - a));
			bool rightOfBC = std::signbit(BC.cross(normal).dot(point - b));
			bool rightOfCA = std::signbit(CA.cross(normal).dot(point - c));

			int positiveCount = rightOfAB + rightOfBC + rightOfCA;

			return positiveCount == 0 || positiveCount == 3;
		}
	};

public: 
	// Implemented like OpenGL meshes to save memory on large meshes
	std::shared_ptr<material> materialPtr;

	mesh() {}
	mesh(
		std::initializer_list<point3> vertices,
		std::initializer_list<int> indices,
		std::shared_ptr<material> materialPtr
	) :
		vertices(vertices),
		indices(indices),
		materialPtr(materialPtr)
	{
		if (vertices.size() <= 0)
			return;

		aabb.cornerMax = aabb.cornerMin = *vertices.begin();
		// skip first vertex
		for (const auto& vertex : vertices | std::views::drop(1))
			aabb.include(vertex);
	}

	virtual std::optional<hit_record> hit(
		const ray& ray,
		double tMin, double tMax
	) const {
		if (!aabb.contains(ray))
			return {};

		bool hit = false;
		hit_record record;
		// extra parenthesis to avoid namespace clash with max()
		record.t = std::numeric_limits<double>::infinity(); 
		record.materialPtr = materialPtr;

		// find closest intersection
		for (int i = 0; i <= indices.size() - 1; i += 3) {
			triangle triangle(
				vertices[indices[i]],
				vertices[indices[i + 1]],
				vertices[indices[i + 2]]
			);
			 
			double t = triangle.hitPlane(ray);

			// Skip if intersection is behind the ray or we've found a closer
			// one.
			if (t < 0 || t >= record.t)
				continue;

			auto intersection = ray.at(t);
			
			if (!triangle.includesPointOnPlane(intersection))
				continue;

			record.normal = triangle.normal;
			record.t = t;
			record.intersection = intersection;
			hit = true;
		}

		if (!hit)
			return {};

		record.normal = record.normal.unit();
		record.setNormalFromOutwardNormal(ray, record.normal);
		return record;
	}

private:
	std::vector<point3> vertices;
	std::vector<int> indices;
	bounding_box aabb; // Axis Aligned Bounding Box
};