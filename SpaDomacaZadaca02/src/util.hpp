#ifndef UTIL_H
#define UTIL_H

#include <SFML/System/Vector2.hpp>

namespace util {

inline sf::Vector2i float_vector_to_integer(const sf::Vector2f vf) {
	return { static_cast<int>(vf.x), static_cast<int>(vf.y) };
}

inline sf::Vector2i integer_vector_to_float(const sf::Vector2i vi) {
	return { vi };
}

};

#endif // !UTIL_H
