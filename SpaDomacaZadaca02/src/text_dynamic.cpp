#include "text_dynamic.hpp"

using namespace malaise::text;

TextDynamic::TextDynamic(const sf::Font &font_, sf::Text base_) : font(font_), base(std::move(base_)) {}

sf::Text& TextDynamic::get_text_template() {
	return base;
}

void TextDynamic::set_position(sf::Vector2f vec) {
	position = std::move(vec);
}

void TextDynamic::set_next(TextDynamic &next_) {
	next = &next_;
}

sf::Text& TextDynamic::get_segment(size_t index) {
	return segments.at(index);
}

void TextDynamic::push_text_segment(const sf::Text& text) {
	segments.push_back(text);
}

void TextDynamic::push_string(const std::string &str) {
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

void TextDynamic::draw(sf::RenderTarget &target) {
	float prev_width = 0.f;
	float prev_height = 0.f;

	for (auto& segment : segments) {
		sf::Vector2f origin_frac = {
			segment.getOrigin().x ? segment.getLocalBounds().width  / segment.getOrigin().x : 0.f,
			segment.getOrigin().y ? segment.getLocalBounds().height / segment.getOrigin().y : 0.f,
		};
		sf::Vector2f bounds_correction = {
			origin_frac.x ? segment.getLocalBounds().width  / origin_frac.x : 0.f,
			origin_frac.y ? segment.getLocalBounds().height / origin_frac.y : 0.f,
		};

		if (segment.getString().toAnsiString().front() == '\n') {
			prev_width = 0.f;

			segment.setPosition(position.x + bounds_correction.x, position.y + prev_height + 3);
			prev_height += segment.getLocalBounds().height + segment.getLineSpacing();
		} else {
			segment.setPosition(position.x + prev_width + bounds_correction.x, position.y + prev_height);
		}
		target.draw(segment);
		prev_width += segment.getLocalBounds().width + segment.getLetterSpacing();
	}
}
