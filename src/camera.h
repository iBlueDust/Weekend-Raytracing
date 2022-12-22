#pragma once

#include "commons.h"
#include "ray.h"
#include "vec3.h"

class camera {
public: 
	camera() {
        auto aspectRatio = 16.0 / 9.0;
        auto viewportHeight = 2.0;
        auto viewportWidth = aspectRatio * viewportHeight;
        auto focalLength = 1.0;

        position = point3(0, 0, 0);
        horizontal = vec3(viewportWidth, 0.0, 0.0);
        vertical = vec3(0.0, viewportHeight, 0.0);
        lowerLeftCorner = position - horizontal / 2 - vertical / 2 - vec3(0, 0, focalLength);
	}

    ray rayFromUV(double u, double v) const {
        return ray(position, lowerLeftCorner + u * horizontal + v * vertical);
    }

private:
    point3 position, lowerLeftCorner;

    // Vectors with magnitudes according to the aspect ratio.
    // One point to the camera's right (horizontal),
    // the other to the camera's up (vertical).
    // Their origin is (0,0,0)
    vec3 horizontal, vertical;
};