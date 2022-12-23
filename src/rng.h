#pragma once

#include <random>

class random_number_generator {
public:
	random_number_generator() : distribution(0.0, 1.0) {}
	random_number_generator(int seed) : distribution(0.0, 1.0), engine(seed) {}

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