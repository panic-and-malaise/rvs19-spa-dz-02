#ifndef MALAISE_TEXT_DYNAMIC_HPP
#define MALAISE_TEXT_DYNAMIC_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

namespace malaise::text {

struct TextSegment {
	sf::Text text;
	size_t start_index = 0;
	size_t length = 0;

	void center_text_origin();
	void text_origin_to_corner();
};

class TextDynamic {
public:
	TextDynamic(const sf::Font &font_, sf::Text base_ = {});

	sf::Text& get_text_template();

	void set_position(sf::Vector2f vec);

	TextSegment& get_segment(size_t index);

	void push_text_segment(const sf::Text& text);

	void push_string(const std::string &str);

	template<typename... Args> // Variadic function, C++17
	void push_strings(const Args&... strings) {
		(push_string(strings), ...);
	}

	void draw(sf::RenderTarget &target);
private:
	sf::Font font;
	sf::Text base; // Reference for construction of text objects

	std::vector<TextSegment> segments;
	sf::Vector2f position{};
};

}

#endif // !MALAISE_TEXT_DYNAMIC_HPP
