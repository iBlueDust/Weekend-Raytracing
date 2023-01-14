#pragma once

#include <random>

class RandomNumberGenerator {
public:
	RandomNumberGenerator() : distribution(0.0, 1.0) {}
	RandomNumberGenerator(int seed) : distribution(0.0, 1.0), engine(seed) {}

	double randomDouble() {
		return distribution(engine);
	}

	double randomDouble(double min, double max) {
		return min + (max - min) * randomDouble();
	}

	// Returns random x between min and max INCLUSIVE
	int randomInt(int min, int max) {
		return static_cast<int>(randomDouble(min, max + 1));
	}

private:
	std::uniform_real_distribution<double> distribution;
	std::mt19937 engine;
};