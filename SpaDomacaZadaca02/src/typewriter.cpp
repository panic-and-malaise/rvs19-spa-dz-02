#include "typewriter.hpp"

namespace anim = malaise::animation;

anim::Typewriter::Typewriter(sf::Text &text_field_, std::string text_, const float speed, std::string cursor_)
	: text_field(text_field_), text(std::move(text_)), chars_per_second(speed), cursor(std::move(cursor_)) {
		if (!cursor.empty()) show_cursor = true;
	}

void anim::Typewriter::set_string(std::string text_) {
	frame_accumulator = 0.f;
	text = std::move(text_);
}

void anim::Typewriter::set_cursor_remain(const bool remain) {
	cursor_remain_after_end = remain;
}

void anim::Typewriter::set_delay(const float delay_) {
	delay = delay_;
}

void anim::Typewriter::update(float delta_time) {
	if (!cursor_remain_after_end && text_field.getString() == text) return;

	if (delay > 0.f) {
		delay -= delta_time;
	} else {
		frame_accumulator += delta_time;
	}

	size_t num_chars = frame_accumulator * chars_per_second;
	num_chars = std::min(num_chars, text.size());
	std::string visible = text.substr(0, num_chars);

	bool show_cursor = std::fmod(frame_accumulator, 1.f) < 0.5f;
	if (show_cursor) visible += cursor;

	text_field.setString(visible);
}
