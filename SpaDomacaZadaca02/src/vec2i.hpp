#ifndef VEC2I_H
#define VEC2I_H

#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <cstdint>

struct Vec2i {
	Vec2i() = default;
	Vec2i(const int32_t x_, const int32_t y_) : x(x_), y(y_) {}
	Vec2i(const sf::Vector2i vec) : x(vec.x), y(vec.y) {}
	Vec2i(const sf::Vector2f vec) : x(vec.x), y(vec.y) {}

	int32_t x{}, y{};

	Vec2i operator+(const Vec2i& other) const;
	Vec2i operator-(const Vec2i& other) const;
	void operator+=(const Vec2i& other);
	Vec2i operator*(const int scalar) const;

	bool operator==(const Vec2i& v) const;

	void normalize();

	float length() const;
	float length_squared() const;
};

struct Vec2iHash {
	size_t operator()(const Vec2i& v) const;
};

#endif // !VEC2I_H
