#include "text_dynamic.hpp"

using namespace malaise::text;

void TextSegment::center_text_origin() {
	text.setOrigin(text.getLocalBounds().width / 2, text.getLocalBounds().height / 2);
}

void TextSegment::text_origin_to_corner() {
	text.setOrigin(0.f, 0.f);
}

TextDynamic::TextDynamic(const sf::Font &font_, sf::Text base_) : font(font_), base(std::move(base_)) {}

sf::Text& TextDynamic::get_text_template() {
	return base;
}

void TextDynamic::set_position(sf::Vector2f vec) {
	position = std::move(vec);
}

TextSegment& TextDynamic::get_segment(size_t index) {
	return segments.at(index);
}

void TextDynamic::push_text_segment(const sf::Text& text) {
	TextSegment segment = {
		text,
		segments.empty() ? 0 : segments.back().start_index + segments.back().length - 1,
		text.getString().getSize()
	};
	segments.push_back(segment);
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
