#pragma once

#include "ray.h"
#include "vec3.h"

// Axis-Aligned Bounding Box (AABB)
class bounding_box {
private:
	class rectangle {
	public:
		double xMin, yMin;
		double xMax, yMax;

		rectangle() {}
		rectangle(
			double xMin, double yMin, double xMax, double yMax
		) : xMin(xMin), yMin(yMin), xMax(xMax), yMax(yMax) {}

		bool contains(double x, double y) const {
			return xMin <= x && xMax >= x
				&& yMin <= y && yMax >= y;
		}
	};

	enum axes {
		x = 0,
		y = 1,
		z = 2
	};

	std::pair<axes, axes> axesPerpendicularTo(axes axis) const {
		constexpr std::pair<axes, axes> table[3] = {
			{ axes::y, axes::z },
			{ axes::x, axes::z },
			{ axes::x, axes::y },
		};

		return table[axis];
	};

	bool doesRayIntersectAxisAlignedRectangle(
		const ray& ray,
		axes axis,
		const rectangle& rectangle,
		double rectanglePosition
	) const {
		double origin = ray.origin.elements[axis];
		double direction = ray.direction.elements[axis];
		double t = (rectanglePosition - origin) / direction;

		// Check if rectangle is behind the ray
		if (t <= 0)
			return false;

		point3 intersection = ray.at(t);

		auto perpendicularAxes = axesPerpendicularTo(axis);
		double xInRectangleSpace = intersection.elements[perpendicularAxes.first];
		double yInRectangleSpace = intersection.elements[perpendicularAxes.second];

		return rectangle.contains(xInRectangleSpace, yInRectangleSpace);
	}

public:
	bounding_box() : cornerMin(0), cornerMax(0) {}
	bounding_box(point3 corner1, point3 corner2) :
		cornerMin(
			std::min<double>(corner1.x, corner2.x),
			std::min<double>(corner1.y, corner2.y),
			std::min<double>(corner1.z, corner2.z)
		),
		cornerMax(
			std::max<double>(corner1.x, corner2.x),
			std::max<double>(corner1.y, corner2.y),
			std::max<double>(corner1.z, corner2.z)
		) {}

	bool contains(const point3& point) const {
		return cornerMin.x <= point.x && cornerMax.x >= point.x
			&& cornerMin.y <= point.y && cornerMax.y >= point.y
			&& cornerMin.z <= point.z && cornerMax.z >= point.z;
	}

	bool contains(const ray& ray) const {
		// xRectangle means it is perpendicular to the x axis, and so forth
		auto xRectangle = rectangle(cornerMin.y, cornerMin.z, cornerMax.y, cornerMax.z);
		auto yRectangle = rectangle(cornerMin.x, cornerMin.z, cornerMax.x, cornerMax.z);
		auto zRectangle = rectangle(cornerMin.x, cornerMin.y, cornerMax.x, cornerMax.y);
		return doesRayIntersectAxisAlignedRectangle(ray, axes::x, xRectangle, cornerMin.x)
			|| doesRayIntersectAxisAlignedRectangle(ray, axes::x, xRectangle, cornerMax.x)
			|| doesRayIntersectAxisAlignedRectangle(ray, axes::y, yRectangle, cornerMin.y)
			|| doesRayIntersectAxisAlignedRectangle(ray, axes::y, yRectangle, cornerMax.y)
			|| doesRayIntersectAxisAlignedRectangle(ray, axes::z, zRectangle, cornerMin.z)
			|| doesRayIntersectAxisAlignedRectangle(ray, axes::z, zRectangle, cornerMax.z);
	}

	void include(const point3& point) {
		cornerMin.x = std::min<double>(cornerMin.x, point.x);
		cornerMin.y = std::min<double>(cornerMin.y, point.y);
		cornerMin.z = std::min<double>(cornerMin.z, point.z);

		cornerMax.x = std::max<double>(cornerMax.x, point.x);
		cornerMax.y = std::max<double>(cornerMax.y, point.y);
		cornerMax.z = std::max<double>(cornerMax.z, point.z);
	}

	// It is always true that
	// cornerMin.x <= cornerMax.x
	// cornerMin.y <= cornerMax.y
	// cornerMin.z <= cornerMax.z
	point3 cornerMin;
	point3 cornerMax;
};

