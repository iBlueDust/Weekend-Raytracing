#pragma once

#include <numbers>
#include <random>

#include "rng.h"

random_number_generator globalRng;

inline double degreesToRadians(double degrees) {
	return degrees * std::numbers::pi / 180.0;
}