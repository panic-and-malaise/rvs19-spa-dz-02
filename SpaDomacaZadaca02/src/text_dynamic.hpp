#ifndef MALAISE_TEXT_DYNAMIC_HPP
#define MALAISE_TEXT_DYNAMIC_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

namespace malaise::text {

class TextDynamic {
public:
	TextDynamic(const sf::Font &font_, sf::Text base_ = {});

	sf::Text& get_text_template();

	void set_position(sf::Vector2f vec);
	sf::Vector2f get_position() const;

	void push_text_segment(const sf::Text& text);
	sf::Text& get_segment(size_t index);

	void push_string(const std::string &str);
	template<typename... Args> // Variadic function, C++17
	void push_strings(const Args&... strings) {
		(push_string(strings), ...);
	}

	void draw(sf::RenderTarget &target);
private:
	sf::Font font;
	sf::Text base; // Reference for construction of text objects

	std::vector<sf::Text> segments;
	sf::Vector2f position{};
};

}

#endif // !MALAISE_TEXT_DYNAMIC_HPP
