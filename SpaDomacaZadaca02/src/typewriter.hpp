#ifndef MALAISE_TYPEWRITER_HPP
#define MALAISE_TYPEWRITER_HPP

#include <string>

#include <SFML/Graphics/Text.hpp>

namespace malaise::animation {

class Typewriter {
public:
	Typewriter(sf::Text &text_field_, std::string text_, const float speed = 1.f, std::string cursor_ = "");

	void set_string(std::string text_);

	void set_cursor_remain(const bool remain);

	void set_delay(const float delay_);

	void update(float delta_time);
private:
	sf::Text &text_field;
	std::string text{};

	float frame_accumulator = 0.f;
	float chars_per_second = 1.f;
	float delay = -1.f;

	std::string cursor = "|";
	bool show_cursor = false;
	bool cursor_remain_after_end = false;
};

}

#endif // !MALAISE_TYPEWRITER_HPP
