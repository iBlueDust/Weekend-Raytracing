#pragma once

#include <functional>
#include <memory>
#include <optional>

#include "bounding_box.h"
#include "commons.h"
#include "hittable.h"
#include "hittable_list.h"

// Binary Tree of (Axis-Aligned) Bounding Boxes, abbreviated BVH
class BoundingVolumeHierarchyNode : public Hittable {
public:
	BoundingVolumeHierarchyNode() {}

	BoundingVolumeHierarchyNode(
		const HittableList& list, double tStart, double tEnd
	) : BoundingVolumeHierarchyNode(list.hittables, tStart, tEnd) { }

	BoundingVolumeHierarchyNode(
		const std::vector<std::shared_ptr<Hittable>>& list,
		double tStart,
		double tEnd
	) : BoundingVolumeHierarchyNode(
		list, tStart, tEnd, 0, list.size()
	) {	}

	BoundingVolumeHierarchyNode(
		const std::vector<std::shared_ptr<Hittable>>& sourceList,
		const double tStart,
		const double tEnd,
		const size_t start,
		const size_t end
	) {

		auto hittableCount = end - start;
		if (hittableCount == 1) {
			left = right = sourceList[0];

			auto box = left->boundingBox(tStart, tEnd);
			if (!box)
				throw NO_BOX_ERROR;

			aabb = box.value();
			return;
		}

		partitionHittables(sourceList, tStart, tEnd, start, end);
		computeAABB(tStart, tEnd);
	}

	std::optional<HitRecord> hit(
		const Ray& ray, double tMin, double tMax
	) const {
		if (!aabb.hit(ray, tMin, tMax))
			return {};

		auto hitsLeft = left->hit(ray, tMin, tMax);
		if (hitsLeft)
			tMax = hitsLeft.value().t;

		auto hitsRight = right->hit(ray, tMin, tMax);

		if (hitsRight)
			return hitsRight;

		return hitsLeft;
	}

	std::optional<BoundingBox> boundingBox(double tStart, double tEnd) const {
		return aabb;
	}

private:
	// Children nodes
	std::shared_ptr<Hittable> left;
	std::shared_ptr<Hittable> right;

	BoundingBox aabb;

	
	static const std::invalid_argument NO_BOX_ERROR;

	void computeAABB(double tStart, double tEnd) {
		auto leftBox = left->boundingBox(tStart, tEnd);
		auto rightBox = right->boundingBox(tStart, tEnd);
		if (!leftBox || !rightBox)
			throw NO_BOX_ERROR;

		aabb = BoundingBox::merge(leftBox.value(), rightBox.value());
	}

	void partitionHittables(
		const std::vector<std::shared_ptr<Hittable>>& sourceList,
		const double tStart,
		const double tEnd,
		const size_t start,
		const size_t end
	) {
		// TODO: Reevaluate this globalRNG usage
		// Get random axis
		const Axis axis = static_cast<Axis>(globalRng.randomInt(0, 2));
		const auto comparator = makeBoundingBoxComparator(axis, tStart, tEnd);

		auto hittableCount = end - start;

		if (hittableCount == 2) {
			auto hittableA = sourceList[start];
			auto hittableB = sourceList[start + 1];

			if (comparator(hittableA, hittableB)) {
				left = hittableA;
				right = hittableB;
			}
			else {
				left = hittableB;
				right = hittableA;
			}

			return;
		}

		// Copy sourceList
		std::vector<std::shared_ptr<Hittable>> list = sourceList;

		auto listBegin = list.begin();
		std::sort(listBegin + start, listBegin + end, comparator);

		// Recursion
		size_t middle = (start + end) / 2;
		left = std::make_shared<BoundingVolumeHierarchyNode>(
			list, tStart, tEnd, start, middle
		);
		right = std::make_shared<BoundingVolumeHierarchyNode>(
			list, tStart, tEnd, middle, end
		);
	}

	std::function<
		bool(
			const std::shared_ptr<Hittable>, 
			const std::shared_ptr<Hittable>
			)
	> makeBoundingBoxComparator(Axis axis, double tStart, double tEnd) {
		return [=](
			const std::shared_ptr<Hittable> a,
			const std::shared_ptr<Hittable> b
			) {
				auto boxA = a->boundingBox(tStart, tEnd);
				if (!boxA)
					throw NO_BOX_ERROR;

				auto boxB = b->boundingBox(tStart, tEnd);
				if (!boxB)
					throw NO_BOX_ERROR;

				return boxA.value().cornerMin[axis] < boxB.value().cornerMin[axis];
		};
	}
};

const std::invalid_argument BoundingVolumeHierarchyNode::NO_BOX_ERROR = 
	std::invalid_argument("Hittable does not have a Bounding Box");