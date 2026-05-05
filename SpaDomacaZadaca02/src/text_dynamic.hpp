#ifndef MALAISE_TEXT_DYNAMIC_HPP
#define MALAISE_TEXT_DYNAMIC_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

namespace malaise::text {

struct TextSegment {
	sf::Text text;
	size_t start_index = 0;
	size_t length = 0;

	void center_text_origin() {
		text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
	}

	void text_origin_to_corner() {
		text.setOrigin(0.f, 0.f);
	}
};

class TextDynamic {
public:
	TextDynamic(const sf::Font &font_, sf::Text base_ = {}) : font(font_), base(std::move(base_)) {}

	sf::Text& get_text_template() {
		return base;
	}

	void set_position(sf::Vector2f vec) {
		position = std::move(vec);
	}

	TextSegment& get_segment(size_t index) {
		return segments.at(index);
	}

	void push_text_segment(const sf::Text& text) {
		TextSegment segment = {
			text,
			segments.empty() ? 0 : segments.back().start_index + segments.back().length - 1,
			text.getString().getSize()
		};
		segments.push_back(segment);
	}

	void push_string(const std::string &str) {
		sf::Text text_object;

		text_object.setString(str);
		text_object.setFont(font);

		text_object.setLineSpacing(base.getLineSpacing());
		text_object.setLetterSpacing(base.getLetterSpacing());

		text_object.setCharacterSize(base.getCharacterSize());

		text_object.setFillColor(base.getFillColor());
		text_object.setOutlineColor(base.getOutlineColor());

		text_object.setOutlineThickness(base.getOutlineThickness());
		text_object.setStyle(base.getStyle());
		
		text_object.setOrigin(text_object.getLocalBounds().width / 2, text_object.getLocalBounds().height / 2);

		push_text_segment(text_object);
	}


	template<typename... Args> // Variadic function, C++17
	void push_strings(const Args&... strings) {
		(push_string(strings), ...);
	}

	void draw(sf::RenderTarget &target) {
		float prev_width = 0.f;
		float prev_height = 0.f;

		for (auto& segment : segments) {
			sf::Vector2f origin_frac = {
				segment.text.getOrigin().x ? segment.text.getLocalBounds().width  / segment.text.getOrigin().x : 0.f,
				segment.text.getOrigin().y ? segment.text.getLocalBounds().height / segment.text.getOrigin().y : 0.f,
			};
			sf::Vector2f bounds_correction = {
				origin_frac.x ? segment.text.getLocalBounds().width  / origin_frac.x : 0.f,
				origin_frac.y ? segment.text.getLocalBounds().height / origin_frac.y : 0.f,
			};

			if (segment.text.getString().toAnsiString().front() == '\n') {
				prev_width = 0.f;

				segment.text.setPosition(position.x + bounds_correction.x, position.y + prev_height + 3);
				prev_height += segment.text.getLocalBounds().height + segment.text.getLineSpacing();
			} else {
				segment.text.setPosition(position.x + prev_width + bounds_correction.x, position.y + prev_height);
			}
			target.draw(segment.text);
			prev_width += segment.text.getLocalBounds().width + segment.text.getLetterSpacing();
		}
	}
private:
	sf::Font font;
	sf::Text base; // Reference for construction of text objects
	std::vector<TextSegment> segments;
	sf::Vector2f position{};
};

}

#endif // !MALAISE_TEXT_DYNAMIC_HPP
