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

private:
	std::uniform_real_distribution<double> distribution;
	std::mt19937 engine;
};