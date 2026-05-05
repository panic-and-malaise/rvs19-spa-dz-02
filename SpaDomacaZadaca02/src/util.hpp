#ifndef UTIL_H
#define UTIL_H

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#ifndef NDEBUG
#define DEBUG_PRINT(msg) std::cout << msg << "\n"
#else
#define DEBUG_PRINT(msg) ;
#endif

namespace util {

inline sf::Vector2i float_vector_to_integer(const sf::Vector2f vf) {
	return { static_cast<int>(vf.x), static_cast<int>(vf.y) };
}

inline sf::Vector2i integer_vector_to_float(const sf::Vector2i vi) {
	return { vi };
}

inline void center_element(sf::Transformable &element, const sf::FloatRect &bounds) {
	element.setOrigin(bounds.width / 2, bounds.height / 2);
}

};

#endif // !UTIL_H
