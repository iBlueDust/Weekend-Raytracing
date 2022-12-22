#pragma once

#include "commons.h"
#include "ray.h"
#include "vec3.h"

struct camera_config {
    point3 lookFrom;
    point3 lookAt;
    vec3 worldUp;
    double verticalFovInDegrees;
    double aspectRatio;
    double aperture;
    double focalLength;
};

class camera {
public: 
	camera(camera_config config) {
        auto theta = degreesToRadians(config.verticalFovInDegrees);
        auto viewportHeight = 2.0 * std::tan(theta / 2.0);
        auto viewportWidth = config.aspectRatio * viewportHeight;

        forward = (config.lookAt - config.lookFrom).unit();
        right = forward.cross(config.worldUp).unit();
        up = right.cross(forward);

        position = config.lookFrom;
        horizontal = viewportWidth * right * config.focalLength;
        vertical = viewportHeight * up * config.focalLength;
        lowerLeftCorner = 
            position - horizontal / 2 - vertical / 2 
            + forward * config.focalLength;

        lensRadius = config.aperture / 2;
	}

    ray rayFromUV(double screenU, double screenV) const {
        vec3 lensPosition = lensRadius * vec3::randomInUnitDisk();
        vec3 offset = right * lensPosition.x + up * lensPosition.y;

        return ray(
            position + offset, 
            lowerLeftCorner
            + screenU * horizontal 
            + screenV * vertical
            - position - offset
        );
    }

private:
    point3 position, lowerLeftCorner;

    // Vectors with magnitudes according to the aspect ratio.
    // One point to the camera's right (horizontal),
    // the other to the camera's up (vertical).
    // Their origin is (0,0,0)
    vec3 horizontal, vertical;
    vec3 forward, right, up;
    double lensRadius;
};