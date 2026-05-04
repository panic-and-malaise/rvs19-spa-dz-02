#include "vec2f.hpp"

#include <cmath>

Vec2D Vec2D::operator+(const Vec2D& other) const {
	return Vec2D{
		x + other.x,
		y + other.y,
	};
}

Vec2D Vec2D::operator-(const Vec2D& other) const {
	return Vec2D{
		x - other.x,
		y - other.y,
	};
}

void Vec2D::operator+=(const Vec2D& other) {
	x += other.x;
	y += other.y;
}

Vec2D Vec2D::operator*(const float scalar) const {
	return Vec2D{
		x * scalar,
		y * scalar,
	};
}

void Vec2D::normalize() {
	float length = std::sqrtf(x*x + y*y);

	x /= length;
	y /= length;
}

// Best to avoid entirely when possible! Square roots are expensive and take dozens of cycles, even on new CPUs.
float Vec2D::length() const {
    return std::sqrt(x*x + y*y);
}

float Vec2D::length_squared() const {
    return x*x + y*y;
}
