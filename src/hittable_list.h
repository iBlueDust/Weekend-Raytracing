#pragma once

#include <memory>
#include <optional>
#include <ranges>
#include <vector>

#include "hittable.h"


class HittableList : public Hittable {
public:
	std::vector<std::shared_ptr<Hittable>> hittables;

	HittableList() {}
	HittableList(std::initializer_list<std::shared_ptr<Hittable>> hittables) { 
		addMany(hittables);
	}

	void add(std::shared_ptr<Hittable> hittable) {
		hittables.push_back(hittable);
	}

	template<typename Iterator>
	void addMany(const Iterator start, const Iterator end) {
		hittables.insert(hittables.end(), start, end);
	}

	void addMany(const std::initializer_list<std::shared_ptr<Hittable>> newHittables) {
		hittables.insert(hittables.end(), newHittables.begin(), newHittables.end());
	}

	void clear() {
		hittables.clear();
	}

	virtual std::optional<HitRecord> hit
		(const Ray& ray, double tMin, double tMax) const override;

	virtual std::optional<BoundingBox> boundingBox
		(double tStart, double tEnd) const override;
};

std::optional<HitRecord> HittableList::hit(const Ray& ray, double tMin, double tMax) const {
	// minimum parametric value t corresponds to the closest hittable
	// (disregarding those behind the ray)
	double minT = tMax;
	std::optional<HitRecord> closestHit = {};

	for (const auto& hittable : hittables) {
		auto hit = hittable->hit(ray, tMin, tMax);
		if (!hit)
			continue;

		auto record = hit.value();
		if (!closestHit || minT > record.t) {
			minT = record.t;
			closestHit = std::optional<HitRecord>(hit);
		}
	}

	return closestHit;
}

std::optional<BoundingBox> HittableList::boundingBox(
	double tStart, double tEnd
) const {
	if (hittables.empty())
		return {};

	auto firstBoundingBox = hittables[0]->boundingBox(tStart, tEnd);
	if (!firstBoundingBox)
		return {};

	BoundingBox result = firstBoundingBox.value();

	for (const auto& hittable : hittables | std::ranges::views::drop(1)) {
		auto boundingBox = hittable->boundingBox(tStart, tEnd);
		if (!boundingBox.has_value())
			return {};

		result = BoundingBox::merge(result, boundingBox.value());
	}

	return result;
}
