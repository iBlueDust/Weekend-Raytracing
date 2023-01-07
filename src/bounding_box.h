#pragma once

#include "ray.h"
#include "vec3.h"

// Axis-Aligned Bounding Box (AABB)
class BoundingBox {
private:
	class Rectangle {
	public:
		double xMin, yMin;
		double xMax, yMax;

		Rectangle() {}
		Rectangle(
			double xMin, double yMin, double xMax, double yMax
		) : xMin(xMin), yMin(yMin), xMax(xMax), yMax(yMax) {}

		bool contains(double x, double y) const {
			return xMin <= x && xMax >= x
				&& yMin <= y && yMax >= y;
		}
	};

	enum Axis {
		X = 0,
		Y = 1,
		Z = 2
	};

	std::pair<Axis, Axis> axesPerpendicularTo(Axis axis) const {
		constexpr std::pair<Axis, Axis> table[3] = {
			{ Axis::Y, Axis::Z },
			{ Axis::X, Axis::Z },
			{ Axis::X, Axis::Y },
		};

		return table[axis];
	};

	bool doesRayIntersectAxisAlignedRectangle(
		const Ray& ray,
		Axis axis,
		const Rectangle& rectangle,
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
	BoundingBox() : cornerMin(0), cornerMax(0) {}
	BoundingBox(point3 corner1, point3 corner2) :
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

	bool contains(const Ray& ray) const {
		// xRectangle means it is perpendicular to the x axis, and so forth
		auto xRectangle = Rectangle(cornerMin.y, cornerMin.z, cornerMax.y, cornerMax.z);
		auto yRectangle = Rectangle(cornerMin.x, cornerMin.z, cornerMax.x, cornerMax.z);
		auto zRectangle = Rectangle(cornerMin.x, cornerMin.y, cornerMax.x, cornerMax.y);
		return doesRayIntersectAxisAlignedRectangle(ray, Axis::X, xRectangle, cornerMin.x)
			|| doesRayIntersectAxisAlignedRectangle(ray, Axis::X, xRectangle, cornerMax.x)
			|| doesRayIntersectAxisAlignedRectangle(ray, Axis::Y, yRectangle, cornerMin.y)
			|| doesRayIntersectAxisAlignedRectangle(ray, Axis::Y, yRectangle, cornerMax.y)
			|| doesRayIntersectAxisAlignedRectangle(ray, Axis::Z, zRectangle, cornerMin.z)
			|| doesRayIntersectAxisAlignedRectangle(ray, Axis::Z, zRectangle, cornerMax.z);
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

