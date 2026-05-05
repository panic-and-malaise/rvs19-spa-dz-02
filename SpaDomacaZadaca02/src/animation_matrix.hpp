#ifndef MALAISE_ANIMATION_MATRIX_HPP
#define MALAISE_ANIMATION_MATRIX_HPP

#include "animation.hpp"
#include "animation_queue.hpp"

namespace malaise::animation {

class AnimationMatrix {
public:
	// Push to the last created animation queue
	void push_current(const Animation &anim);
	// Push to a newly created animation queue at the back of the vector
	void push_and_create(const Animation &anim);
	// Push to a specific animation queue at an index
	void push_to_index(const Animation &anim, const size_t index);

	AnimationQueue& at(const size_t index);

	std::vector<AnimationQueue>& get_animations();
private:
	std::vector<AnimationQueue> animations{};
};

}

#endif // !MALAISE_ANIMATION_MATRIX_HPP
