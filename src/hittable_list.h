#pragma once

#include <memory>
#include <optional>
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

	virtual std::optional<HitRecord> hit(const Ray& ray, double tMin, double tMax) const override;
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
