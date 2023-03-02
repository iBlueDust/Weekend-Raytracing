#pragma once

#include <cmath>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <vector>

#include "bounding_box.h"
#include "bounding_volume_hierarchy.h"
#include "hittable.h"
#include "material.h"
#include "triangle.h"
#include "vec3.h"

// TODO: implement BVH on Mesh as well

class Mesh : public Hittable {
private:
	std::vector<point3> vertices;
	std::vector<int> indices;
	std::vector<std::shared_ptr<Material>> materialPtrs;
	std::vector<int> materialIndices;

	BoundingBox aabb; // Axis Aligned Bounding Box
	BoundingVolumeHierarchyNode bvh;

public: 

	Mesh() {}
	
	Mesh(
		const std::initializer_list<point3>& vertices,
		const std::initializer_list<int>& indices,
		const std::shared_ptr<Material>& materialPtr
	) : Mesh(vertices, indices, { materialPtr }, {}) {}

	Mesh(
		const std::initializer_list<point3>& vertices,
		const std::initializer_list<int>& indices,
		const std::initializer_list<std::shared_ptr<Material>>& materialPtrs,
		const std::initializer_list<int>& materialIndices
	) :
		vertices(vertices),
		indices(indices),
		materialPtrs(materialPtrs),
		materialIndices(materialIndices)
	{
		if (vertices.size() <= 3)
			throw std::invalid_argument(
				"Mesh requires at least 3 vertices for a triangle"
			);
		if (materialPtrs.size() <= 0)
			throw std::invalid_argument(
				"Mesh requires at least one material pointer"
			);		

		// Precalculate bounding box
		bvh = generateBVH(vertices, indices, materialPtrs, materialIndices);
		aabb = bvh.boundingBox(0.0, 0.0).value();
	}

	typedef Hittable super;

	virtual std::optional<HitRecord> hit(
		const Ray& ray,
		double tMin, double tMax
	) const {
		return bvh.hit(ray, tMin, tMax);
	}

	virtual std::optional<BoundingBox> boundingBox(
		double tStart, double tEnd
	) const {
		return aabb;
	}

private:

	BoundingVolumeHierarchyNode generateBVH(
		const std::vector<point3>& vertexs,
		const std::vector<int>& indices,
		const std::vector<std::shared_ptr<Material>>& materialPtrs,
		const std::vector<int>& materialIndices
	) {
		// Work around initializer_lists not having [] operators

		std::vector<std::shared_ptr<Hittable>> triangles;
		triangles.reserve(indices.size() / 3);

		int triangleCount = 0;
		for (int i = 0; i < indices.size(); i += 3) {

			int materialIndex = 
				triangleCount < materialIndices.size()
				? materialIndices[triangleCount]
				: 0;

			Triangle triangle(
				vertices[indices[i]],
				vertices[indices[i + 1]],
				vertices[indices[i + 2]],
				materialPtrs[materialIndex]
			);
			triangles.push_back(std::make_shared<Triangle>(triangle));
			triangleCount++;
		}

		return BoundingVolumeHierarchyNode(triangles, 0.0, 0.0);
	}
};