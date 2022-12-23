#pragma once

#include "vec3.h"

#include <algorithm>
#include <cmath>
#include <iostream>

void writePixel(std::ostream& stream, color3 pixel) {
	stream << static_cast<int>(255.999 * pixel.x) << ' '
	       << static_cast<int>(255.999 * pixel.y) << ' '
	       << static_cast<int>(255.999 * pixel.z) << '\n';
}

color3 gammaCorrect(const color3& pixel) {
	return color3(
		std::sqrt(pixel.r),
		std::sqrt(pixel.g),
		std::sqrt(pixel.b)
	);
}