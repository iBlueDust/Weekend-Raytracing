#pragma once

#include "vec3.h"

#include <algorithm>
#include <iostream>

void writePixel(std::ostream& stream, color3 pixel) {
	stream << static_cast<int>(255.999 * pixel.x) << ' '
	       << static_cast<int>(255.999 * pixel.y) << ' '
	       << static_cast<int>(255.999 * pixel.z) << '\n';
}

void writePixel(std::ostream& stream, color3 pixel, int sampleCount) {
	pixel /= sampleCount;

	pixel.x = std::clamp(pixel.x, 0.0, 0.999);
	pixel.y = std::clamp(pixel.y, 0.0, 0.999);
	pixel.z = std::clamp(pixel.z, 0.0, 0.999);

	writePixel(stream, pixel);
}