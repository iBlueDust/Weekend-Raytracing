#pragma once

#include "vec3.h"

#include <iostream>

void writePixel(std::ostream& stream, color3 pixel) {
	stream << static_cast<int>(255.999 * pixel.x) << ' '
	       << static_cast<int>(255.999 * pixel.y) << ' '
	       << static_cast<int>(255.999 * pixel.z) << '\n';
}