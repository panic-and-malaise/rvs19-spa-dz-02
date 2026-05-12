#ifndef MALAISE_ANIMATION_HPP
#define MALAISE_ANIMATION_HPP

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

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
			constexpr float elasticity_coefficient = 1.70158f;
			return t * t * ((elasticity_coefficient + 1) * t - elasticity_coefficient);
		}
		default:
			return t;
	}
}

// Have to declare here to avoid circular include hell
struct Keyframe {
	Keyframe(const float start_time_, sf::Vector2f pos_, const float rotation_, sf::Vector2f scale_ = { 1.f, 1.f }, const uint8_t alpha_ = 255, const Interpolation interpolation_ = Interpolation::LINEAR)
		: position(std::move(pos_)), rotation(rotation_), scale(std::move(scale_)), start_time(start_time_), alpha(alpha_), interpolation(interpolation_) {}

	float start_time = 0.f;
	float rotation = 0.f;
	sf::Vector2f position{};
	sf::Vector2f scale{};
	uint8_t alpha = 255;
	Interpolation interpolation = Interpolation::LINEAR;
};

class Animation {
public:
	/*
	 * Create a new Animation from a sf::Transformable object and create an initial keyframe.
	 * The initial keyframe will always start immediately, at 0.f, and inherit all of the
	 * sf::Transformable object's transforms from the moment the Animation was constructed.
	 */
    Animation(std::shared_ptr<sf::Transformable> t, const bool loop_ = false) : object(t), looping(loop_) {
		init_first_keyframe();
	}

	/*
	 * Create an initial keyframe based on the transforms of the sf::Transformable object.
	 * Used to create a starting point for Interpolation, and conveniently record the
	 * initial position for later copying, such as through the method below.
	 */
	void init_first_keyframe();

	/*
	 * Copy the initial, auto-created keyframe to the end of the Keyframe vector,
	 * at the specified start_time. Very useful for creating looping animations.
	 */
	void copy_first_keyframe_to_last(const float start_time_);

	void set_looping(const bool loop_);
	bool is_finished() const;

	/*
	 * Whether the shared_ptr reference to the object has been destructed
	 */
	bool expired() const;

	void put_keyframe(Keyframe keyframe);
	void put_keyframe(const float start_time_, const sf::Vector2f pos_, const float rotation_, const sf::Vector2f scale_ = { 1.f, 1.f }, const uint8_t alpha_ = 255, const Interpolation interpolation_ = Interpolation::LINEAR);
	void put_keyframe_idle(const float start_time_);

	/*
	 * Apply Keyframe transforms to sf::Transformable object directly, with no interpolation.
	 */
	void apply_keyframe(const Keyframe &key);

	/*
	 * Apply Keyframe transforms to sf::Transformable object,
	 * using the Interpolation from the &next keyframe.
	 */
	void apply_keyframe_interp(const Keyframe &prev, const Keyframe &next, float current_time);

	void update(float delta_time);
private:
	/*
	 * std::shared_ptr reference to the sf::Transformable object.
	 * Used to apply transforms and manage own lifetime.
	 */
	std::shared_ptr<sf::Transformable> object;
	std::vector<Keyframe> keyframes{};

	/*
	 * Counts the Animation's time and progress by accumulating from delta_time.
	 * Is equal to the number of seconds elapsed as a floating point decimal number.
	 */
	float frame_accumulator = 0.f;
	/*
	 * The current Animation Keyframe being applied or Interpolated to.
	 * Value ranges from 0, to one plus the size of the Keyframe vector,
	 * when it effectively immediately returns from all update methods.
	 */
	size_t current_frame = 0;

	bool finished = false;
	bool looping = false;
};

inline Animation animation_idle(std::shared_ptr<sf::Transformable> transformable, const float duration = 1.5f) {
	Animation idle(transformable);
	idle.copy_first_keyframe_to_last(duration);
	return idle;
}

inline Animation animation_idle_pop(std::shared_ptr<sf::Transformable> transformable) {
	Animation idle_pop(transformable, true);
	idle_pop.put_keyframe(1.f, {}, -30.f, {2.f, 2.f}, 255, malaise::animation::Interpolation::EASE_OUT);
	idle_pop.copy_first_keyframe_to_last(1.5f);
	return idle_pop;
}

inline Animation animation_idle_shake(std::shared_ptr<sf::Transformable> transformable) {
	Animation idle_pop(transformable, true);
	idle_pop.copy_first_keyframe_to_last(.5f);
	idle_pop.put_keyframe(1.f, {}, -10.f, {1.1f, 1.1f}, 255, malaise::animation::Interpolation::EASE_IN_OUT);
	idle_pop.copy_first_keyframe_to_last(1.5f);
	return idle_pop;
}

}

#endif // !MALAISE_ANIMATION_HPP
