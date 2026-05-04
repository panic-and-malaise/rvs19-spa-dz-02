#include "vec2i.hpp"

#include <cmath>
#include <cstdint>

Vec2i Vec2i::operator+(const Vec2i& other) const {
	return Vec2i{
		x + other.x,
		y + other.y,
	};
}

Vec2i Vec2i::operator-(const Vec2i& other) const {
	return Vec2i{
		x - other.x,
		y - other.y,
	};
}

void Vec2i::operator+=(const Vec2i& other) {
	x += other.x;
	y += other.y;
}

Vec2i Vec2i::operator*(const int scalar) const {
	return Vec2i{
		x * scalar,
		y * scalar,
	};
}

// Hashing function, literally just combines the bits of x and y into a single 64-bit integer,
// should work nicely with a set implementation
size_t Vec2iHash::operator()(const Vec2i& v) const {
	return int64_t(v.y << sizeof(int32_t) | v.x);
}

bool Vec2i::operator==(const Vec2i& v) const {
	return x == v.x && y == v.y;
}

void Vec2i::normalize() {
	float length = std::sqrtf(x*x + y*y);

	x /= length;
	y /= length;
}

// Best to avoid entirely when possible! Square roots are expensive and take dozens of cycles, even on new CPUs.
float Vec2i::length() const {
    return std::sqrt(x*x + y*y);
}

float Vec2i::length_squared() const {
    return x*x + y*y;
}
