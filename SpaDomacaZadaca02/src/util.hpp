#ifndef MALAISE_UTIL_HPP
#define MALAISE_UTIL_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#ifndef NDEBUG // Automagic define that the compiler sets based on the release mode
#define DEBUG_PRINT(msg) std::cout << msg << "\n"
#else
#define DEBUG_PRINT(msg) ;
#endif

namespace malaise::util {

inline sf::Vector2i float_vector_to_integer(const sf::Vector2f vf) {
	return { static_cast<int>(vf.x), static_cast<int>(vf.y) };
}

inline sf::Vector2f integer_vector_to_float(const sf::Vector2i vi) {
	return { static_cast<float>(vi.x), static_cast<float>(vi.y) };
}

inline void center_element(sf::Transformable &element, const sf::FloatRect &bounds) {
	element.setOrigin(bounds.width / 2, bounds.height / 2);
}

};

#endif // !MALAISE_UTIL_HPP
