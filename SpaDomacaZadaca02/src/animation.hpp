#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace Animation {

inline void rotate_object_centered(sf::Transformable &tf, float radian) {
	tf.setOrigin(1, 1);
	tf.setRotation(radian);
}

enum class Interpolation {
	NONE = 0,
	LINEAR,
	QUADRATIC,
	CUBIC,
	EASE_IN,
	EASE_OUT,
	EASE_IN_OUT,
	ELASTIC,
};

inline float apply_interpolation(float t, Animation::Interpolation type) {
	switch (type) {
		case Animation::Interpolation::LINEAR:
			return t;
		case Animation::Interpolation::QUADRATIC:
			return t * t;
		case Animation::Interpolation::CUBIC:
			return t * t * t;
		case Animation::Interpolation::EASE_IN:
			return t * t;
		case Animation::Interpolation::EASE_OUT:
			return 1.0f - (1.0f - t) * (1.0f - t);
		case Animation::Interpolation::EASE_IN_OUT:
			return t * t * (3.0f - 2.0f * t);
		case Animation::Interpolation::ELASTIC: {
			constexpr float s = 1.70158f;
			return t * t * ((s + 1) * t - s);
		}
		default:
			return t;
	}
}

struct Keyframe {
	Keyframe(const float start_time_, const sf::Vector2f pos_, const float rotation_, const sf::Vector2f scale_ = { 1.f, 1.f }, const Interpolation interpolation_ = Interpolation::LINEAR)
		: position(std::move(pos_)), rotation(rotation_), scale(std::move(scale_)), start_time(start_time_), interpolation(interpolation_) {}

	sf::Vector2f position{};
	float rotation = 0.0f;
	sf::Vector2f scale{};
	float start_time = 0.0f;
	Interpolation interpolation = Interpolation::LINEAR;
};

class Animation {
public:
    Animation(sf::Transformable &t, const bool loop_ = false) : object(t), looping(loop_) {
		init_first_keyframe();
	}

	void init_first_keyframe();
	void copy_first_keyframe_to_last(const float start_time_);

	void set_looping(const bool loop_);
	bool is_finished() const;

	void put_keyframe(Keyframe keyframe);
	void put_keyframe(const float start_time_, const sf::Vector2f pos_, const float rotation_, const sf::Vector2f scale_ = { 1.f, 1.f }, const Interpolation interpolation_ = Interpolation::LINEAR);
	void put_keyframe_idle(const float start_time_);

	void apply_keyframe(Keyframe &key);
	void apply_keyframe_interp(Keyframe &prev, Keyframe &next, float current_time);

	void update(float delta_time);
private:
	sf::Transformable &object;
	std::vector<Keyframe> keyframes{};

	float frame_accumulator = 0.0f;
	size_t current_frame = 0;

	bool finished = false;
	bool looping = false;
};

class Typewriter {
public:
	Typewriter(sf::Text &text_field_, std::string text_, const float speed = 1.0f, std::string cursor_ = "")
		: text_field(text_field_), text(std::move(text_)), chars_per_second(speed), cursor(std::move(cursor_)) {
			if (!cursor.empty()) show_cursor = true;
		}

	void set_string(std::string text_) {
		frame_accumulator = 0.0f;
		text = std::move(text_);
	}

	void set_cursor_remain(const bool remain) {
		cursor_remain_after_end = remain;
	}

	void update(float delta_time) {
		if (!cursor_remain_after_end && text_field.getString() == text) return;

		frame_accumulator += delta_time;

		size_t num_chars = frame_accumulator * chars_per_second;
		num_chars = std::min(num_chars, text.size());
		std::string visible = text.substr(0, num_chars);

		bool show_cursor = std::fmod(frame_accumulator, 1.0f) < 0.5f;
		if (show_cursor) visible += cursor;

		text_field.setString(visible);
	}
private:
	sf::Text &text_field;
	std::string text{};

	float frame_accumulator = 0.0f;
	float chars_per_second = 1.0f;

	std::string cursor = "|";
	bool show_cursor = false;
	bool cursor_remain_after_end = false;
};

struct TextSegment {
	sf::Text text;
	size_t start_index = 0;
	size_t length = 0;
};

class TextDynamic {
public:
	void set_position(sf::Vector2f vec) {
		position = std::move(vec);
	}

	void push_text_segment(const sf::Text& text) {
		TextSegment segment = {
			text,
			segments.empty() ? 0 : segments.back().start_index + segments.back().length - 1,
			text.getString().getSize()
		};
		segments.emplace_back(segment);
	}

	void update(float delta_time) {
		
	}

	void draw(sf::RenderTarget &target) {
		float prev_width = 0.0f;
		for (auto& segment : segments) {
			// std::cout << "Drawing segment: " << segment.text.getString().toAnsiString() << "\n";
			segment.text.setPosition(position.x + prev_width, position.y);
			target.draw(segment.text);
			prev_width += segment.text.getLocalBounds().width;
		}
	}
private:
	std::vector<TextSegment> segments;
	sf::Vector2f position{};
};

}

#endif // !ANIMATION_HPP
