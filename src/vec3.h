#pragma once

#include <math.h>

#include "commons.h"

class vec3 {
public:
	union {
		struct { double x, y, z; };
		struct { double r, g, b; };
		double elements[3];
	};
	

	vec3() : elements{ 0, 0, 0 } {}
	vec3(double a) : elements{ a, a, a } {}
	vec3(double x, double y, double z) : elements{ x, y, z } {}

	double operator[](int i) const { return elements[i]; }
	double& operator[](int i) { return elements[i]; }

	// Operators
	inline vec3 operator+() const { return *this; }
	inline vec3 operator-() const { return std::move(vec3(-x, -y, -z)); }

	vec3& operator+=(const vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator-=(const vec3& v) {
		*this += -v;
		return *this;
	}

	vec3& operator*=(const double scale) {
		x *= scale;
		y *= scale;
		z *= scale;
		return *this;
	}

	vec3& operator*=(const vec3& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3& operator/=(const double scale) {
		*this *= 1.0 / scale;
		return *this;
	}

	// Utility functions
	inline double dot(const vec3 &b) const {
		return x * b.x + y * b.y + z * b.z;
	}

	inline vec3 cross(const vec3 &b) const {
		return std::move(
			vec3(
				y * b.z - z * b.y,
				z * b.x - x * b.z,
				x * b.y - y * b.x
			)
		);
	}

	// Declare other methods (definition below)
	
	inline double squareMagnitude() const;
	inline double magnitude() const;
	inline vec3 unit() const;
	inline bool nearZero() const;
	vec3 reflect(const vec3& normal) const;
	vec3 refract(const vec3& normal, double iorRatio) const;

	// Declare static methods (definition below)
	
	static vec3 lerp(const vec3& a, const vec3& b, const double t);
	static vec3 random(RandomNumberGenerator& rng);
	static vec3 random(RandomNumberGenerator& rng, double min, double max);
	static vec3 randomInUnitSphere(RandomNumberGenerator& rng);
	static vec3 randomOnUnitSphere(RandomNumberGenerator& rng);
	static vec3 randomInUnitDisk(RandomNumberGenerator& rng);

	void fprint(FILE* stream) {
		fprintf(stream, "(%.3f, %.3f, %.3f)", x, y, z);
	}
};

// Type aliases to prevent arithmetic between colors, geometrical points, etc.
using point3 = vec3;
using color3 = vec3;


// Utility functions

inline vec3 operator+(const vec3& a, const vec3& b) {
	return std::move(
		vec3(
			a.x + b.x,
			a.y + b.y,
			a.z + b.z
		)
	);
}
inline vec3 operator-(const vec3& a, const vec3& b) {
	return a + -b;
}

inline vec3 operator*(const vec3& a, const double scale) {
	return std::move(
		vec3(
			a.x * scale,
			a.y * scale,
			a.z * scale
		)
	);
}

inline vec3 operator*(const double scale, const vec3& a) {
	return a * scale;
}

inline vec3 operator*(const vec3& a, const vec3& b) {
	return std::move(
		vec3(
			a.x * b.x,
			a.y * b.y,
			a.z * b.z
		)
	);
}

inline vec3 operator/(const vec3 a, const double scale) {
	return a * (1.0 / scale);
}


// Other methods
inline double vec3::squareMagnitude() const {
	return this->dot(*this);
}

inline double vec3::magnitude() const {
	return sqrt(squareMagnitude());
}

inline vec3 vec3::unit() const {
	return *this / magnitude();
}

inline bool vec3::nearZero() const {
	constexpr double epsilon = 1e-8;
	return squareMagnitude() < epsilon;
}

vec3 vec3::reflect(const vec3& normal) const {
	auto rayIn = this->unit();
	return rayIn - 2 * rayIn.dot(normal) * normal;
}

vec3 vec3::refract(const vec3& normal, double iorRatio) const {
	auto rayIn = this->unit();
	auto cosTheta = std::min<double>(-rayIn.dot(normal), 1.0);
	vec3 perpendicularComponent = iorRatio * (rayIn + normal * cosTheta);
	vec3 parallelComponent = 
		-std::sqrt(
			std::abs(1.0 - perpendicularComponent.squareMagnitude())
		) * normal;
	return perpendicularComponent + parallelComponent;
}

// Static methods

// Linear interpolation
vec3 vec3::lerp(const vec3& a, const vec3& b, const double t) {
	return (1.0 - t) * a + t * b;
}

vec3 vec3::random(RandomNumberGenerator& rng) {
	return vec3(rng.randomDouble(), rng.randomDouble(), rng.randomDouble());
}

vec3 vec3::random(RandomNumberGenerator& rng, double min, double max) {
	return vec3::random(rng) * (max - min) + vec3(min);
}

// Read details:
// http://extremelearning.com.au/how-to-generate-uniformly-random-points-on-n-spheres-and-n-balls/
vec3 vec3::randomInUnitSphere(RandomNumberGenerator& rng) {
	auto radius = std::cbrt(rng.randomDouble());
	return radius * vec3::randomOnUnitSphere(rng);
}

vec3 vec3::randomOnUnitSphere(RandomNumberGenerator& rng) {
	auto cosTheta = rng.randomDouble(-1.0, 1.0);
	auto sinTheta = std::sqrt(1 - cosTheta * cosTheta);

	auto phi = rng.randomDouble(0.0, 2 * std::numbers::pi);
	return vec3(
		std::cos(phi) * sinTheta, // stops samples from gathering at the poles
		std::sin(phi) * sinTheta,
		cosTheta
	);
}

vec3 vec3::randomInUnitDisk(RandomNumberGenerator& rng) {
	auto radius = std::sqrt(rng.randomDouble());
	auto theta = rng.randomDouble(0.0, 2 * std::numbers::pi);
	return vec3(
		radius * std::cos(theta),
		radius * std::sin(theta),
		0.0
	);
}

