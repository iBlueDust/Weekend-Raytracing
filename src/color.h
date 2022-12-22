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

void writePixel(std::ostream& stream, color3 pixel, int sampleCount) {
	pixel *= 1.0 / sampleCount;

	pixel.x = std::clamp(std::sqrt(pixel.x), 0.0, 1.0);
	pixel.y = std::clamp(std::sqrt(pixel.y), 0.0, 1.0);
	pixel.z = std::clamp(std::sqrt(pixel.z), 0.0, 1.0);

	writePixel(stream, pixel);
}