#pragma once

#include "commons.h"
#include "ray.h"
#include "vec3.h"

class camera {
public: 
	camera(
        point3 lookFrom,
        point3 lookAt,
        vec3 worldUp,
        double verticalFovInDegrees, 
        double aspectRatio
    ) {
        auto theta = degreesToRadians(verticalFovInDegrees);
        auto viewportHeight = 2.0 * std::tan(theta / 2.0);
        auto viewportWidth = aspectRatio * viewportHeight;
        auto focalLength = 1.0;

        auto backward = (lookFrom - lookAt).unit();
        auto right = worldUp.cross(backward).unit();
        auto up = backward.cross(right);

        position = lookFrom;
        horizontal = viewportWidth * right;
        vertical = viewportHeight * up;
        lowerLeftCorner = 
            position - horizontal / 2 - vertical / 2 
            - backward * focalLength;
	}

    ray rayFromUV(double screenU, double screenV) const {
        return ray(
            position, 
            lowerLeftCorner + screenU * horizontal + screenV * vertical - position
        );
    }

private:
    point3 position, lowerLeftCorner;

    // Vectors with magnitudes according to the aspect ratio.
    // One point to the camera's right (horizontal),
    // the other to the camera's up (vertical).
    // Their origin is (0,0,0)
    vec3 horizontal, vertical;
};