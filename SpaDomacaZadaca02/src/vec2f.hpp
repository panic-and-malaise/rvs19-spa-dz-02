#ifndef VEC2D_H
#define VEC2D_H

#include <cstddef>
struct Vec2D {
	float x{}, y{};

	Vec2D operator+(const Vec2D& other) const;
	Vec2D operator-(const Vec2D& other) const;
	void operator+=(const Vec2D& other);
	Vec2D operator*(const float scalar) const;

	size_t operator()(const Vec2D& v) const;

	void normalize();

	float length() const;
	float length_squared() const;
};

#endif // !VEC2D_H
