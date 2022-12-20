#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "hittable.h"


class hittable_list : public hittable {
public:
	std::vector<std::shared_ptr<hittable>> hittables;

	hittable_list() {}
	hittable_list(std::initializer_list<std::shared_ptr<hittable>> hittables) { 
		addMany(hittables);
	}

	void add(std::shared_ptr<hittable> hittable) {
		hittables.push_back(hittable);
	}

	template<typename Iterator>
	void addMany(const Iterator start, const Iterator end) {
		hittables.insert(hittables.end(), start, end);
	}

	void addMany(const std::initializer_list<std::shared_ptr<hittable>> newHittables) {
		hittables.insert(hittables.end(), newHittables.begin(), newHittables.end());
	}

	void clear() {
		hittables.clear();
	}

	virtual std::optional<hit_record> hit(const ray& ray, double tMin, double tMax) const override;
};

std::optional<hit_record> hittable_list::hit(const ray& ray, double tMin, double tMax) const {
	// minimum parametric value t corresponds to the closest hittable
	// (disregarding those behind the ray)
	double minT = tMax;
	std::optional<hit_record> closestHit = {};

	for (const auto& hittable : hittables) {
		auto record = hittable->hit(ray, tMin, tMax);
		if (!record)
			continue;

		if (!record || minT > record.value().t) {
			minT = record.value().t;
			closestHit = std::optional<hit_record>(record);
		}
	}

	return closestHit;
}
