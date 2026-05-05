#ifndef MALAISE_ANIMATION_HPP
#define MALAISE_ANIMATION_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace malaise::animation {

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

inline float apply_interpolation(float t, animation::Interpolation type) {
	switch (type) {
		case animation::Interpolation::LINEAR:
			return t;
		case animation::Interpolation::QUADRATIC:
			return t * t;
		case animation::Interpolation::CUBIC:
			return t * t * t;
		case animation::Interpolation::EASE_IN:
			return t * t;
		case animation::Interpolation::EASE_OUT:
			return 1.f - (1.f - t) * (1.f - t);
		case animation::Interpolation::EASE_IN_OUT:
			return t * t * (3.f - 2.f * t);
		case animation::Interpolation::ELASTIC: {
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
	float rotation = 0.f;
	sf::Vector2f scale{};
	float start_time = 0.f;
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

	float frame_accumulator = 0.f;
	size_t current_frame = 0;

	bool finished = false;
	bool looping = false;
};

class AnimationQueue {
public:
	void push(const Animation &anim) {
		animation_queue.push(anim);
	}

	void pop() {
		animation_queue.pop();
	}

	Animation& front() {
		return animation_queue.front();
	}

	bool empty() const {
		return animation_queue.empty();
	}
private:
	std::queue<Animation> animation_queue{};
};

class AnimationMatrix {
public:
	// Push to the last created animation queue
	void push_current(const Animation &anim) {
		animations[animations.size() - 1].push(anim);
	}

	// Push to a newly created animation queue at the back of the vector
	void push_and_create(const Animation &anim) {
		size_t index = animations.size();
		if (index >= animations.size()) animations.resize(index + 1);
		animations[index].push(anim);
	}

	// Push to a specific animation queue at an index
	void push_to_index(const Animation &anim, const size_t index) {
		if (index >= animations.size()) return;
		animations[index].push(anim);
	}

	AnimationQueue& at(const size_t index) {
		if (index >= animations.size()) throw std::out_of_range("AnimationQueue not initialized!");
		return animations[index];
	}

	std::vector<AnimationQueue>& get_animations() {
		return animations;
	}
private:
	std::vector<AnimationQueue> animations{};
};

class Typewriter {
public:
	Typewriter(sf::Text &text_field_, std::string text_, const float speed = 1.f, std::string cursor_ = "")
		: text_field(text_field_), text(std::move(text_)), chars_per_second(speed), cursor(std::move(cursor_)) {
			if (!cursor.empty()) show_cursor = true;
		}

	void set_string(std::string text_) {
		frame_accumulator = 0.f;
		text = std::move(text_);
	}

	void set_cursor_remain(const bool remain) {
		cursor_remain_after_end = remain;
	}

	void set_delay(const float delay_) {
		delay = delay_;
	}

	void update(float delta_time) {
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

#endif // !MALAISE_ANIMATION_HPP
