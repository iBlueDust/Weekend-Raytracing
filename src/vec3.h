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

	// Declare static methods (definition below)
	
	static vec3 lerp(const vec3& a, const vec3& b, const double t);
	static vec3 randomInUnitSphere();

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

// Static methods

// Linear interpolation
vec3 vec3::lerp(const vec3& a, const vec3& b, const double t) {
	return (1.0 - t) * a + t * b;
}

vec3 vec3::randomInUnitSphere() {
	while (1) {
		vec3 candidate = vec3(
			randomDouble(-1.0, 1.0), 
			randomDouble(-1.0, 1.0), 
			randomDouble(-1.0, 1.0)
		);
		if (candidate.squareMagnitude() < 1.0)
			return candidate;
	}
}



