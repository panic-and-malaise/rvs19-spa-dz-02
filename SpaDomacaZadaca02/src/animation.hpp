#ifndef MALAISE_ANIMATION_HPP
#define MALAISE_ANIMATION_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

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

// Have to declare here to avoid circular include hell
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

}

#endif // !MALAISE_ANIMATION_HPP
