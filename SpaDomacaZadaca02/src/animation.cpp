#include "animation.hpp"

namespace anim = malaise::animation;

void anim::Animation::init_first_keyframe() {
	put_keyframe(0.0f, object->getPosition(), object->getRotation(), object->getScale());
}

void anim::Animation::copy_first_keyframe_to_last(const float start_time_) {
	auto frame = keyframes.front();
	frame.start_time = start_time_;
	keyframes.emplace_back(frame);
}

void anim::Animation::set_looping(const bool loop_) {
	looping = loop_;
}

bool anim::Animation::is_finished() const {
	return finished;
}

bool anim::Animation::expired() const {
	return !static_cast<bool>(object);
}

void anim::Animation::put_keyframe(Keyframe keyframe) {
	keyframes.push_back(keyframe);
}

void anim::Animation::put_keyframe(const float start_time_, const sf::Vector2f pos_, const float rotation_, const sf::Vector2f scale_, const Interpolation interpolation_) {
	keyframes.emplace_back(start_time_, pos_, rotation_, scale_, interpolation_);
}

void anim::Animation::put_keyframe_idle(const float start_time_) {
	auto last = keyframes.back();
	last.start_time = start_time_;
	keyframes.emplace_back(last);
}

void anim::Animation::apply_keyframe(const Keyframe &key) {
	if (!object) return;

	object->setPosition(key.position);
	object->setRotation(key.rotation);
	object->setScale(key.scale);
}

void anim::Animation::apply_keyframe_interp(const Keyframe &prev, const Keyframe &next, float current_time) {
	if (!object) return;

	float t = (current_time - prev.start_time) / (next.start_time - prev.start_time); // fraction through the animation
	t = std::max(0.0f, std::min(1.0f, t));
	t = apply_interpolation(t, next.interpolation);

	float pos_x = prev.position.x + (next.position.x - prev.position.x) * t;
	float pos_y = prev.position.y + (next.position.y - prev.position.y) * t;
	if (next.position.x < 0.0f)
		pos_x = object->getPosition().x;
	if (next.position.y < 0.0f)
		pos_x = object->getPosition().y;

	float rotation = prev.rotation + (next.rotation - prev.rotation) * t;

	float scale_x = prev.scale.x + (next.scale.x - prev.scale.x) * t;
	float scale_y = prev.scale.y + (next.scale.y - prev.scale.y) * t;

	object->setPosition({pos_x, pos_y});
	object->setRotation(rotation);
	object->setScale(scale_x, scale_y);
}

void anim::Animation::update(float delta_time) {
	if (keyframes.empty() || current_frame > keyframes.size()) return;
	frame_accumulator += delta_time;

	if (keyframes.size() == current_frame) {
		if (looping) {
			current_frame = 0;
			frame_accumulator -= keyframes.back().start_time;
		} else {
			finished = true;
			current_frame++; // Increment so subsequent calls return at the initial condition
			return;
		}
	}

	if (keyframes[current_frame].interpolation != Interpolation::NONE) {
		if (current_frame > 0) {
			apply_keyframe_interp(
				keyframes[current_frame - 1],
				keyframes[current_frame],
				frame_accumulator
			);
		} else if (looping) {
			// wrap-around interpolation
			apply_keyframe_interp(
				keyframes.back(),
				keyframes[0],
				frame_accumulator
			);
		}
	}

	if (frame_accumulator > keyframes[current_frame].start_time) {
		// std::cout << "current frame: " << current_frame << '\n';
		apply_keyframe(keyframes[current_frame++]);
	}
}
