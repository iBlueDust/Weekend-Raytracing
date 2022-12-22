#pragma once

#include <random>

inline double randomDouble() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

// Random number x where min <= x < max
inline double randomDouble(double min, double max) {
	return min + (max - min) * randomDouble();
}