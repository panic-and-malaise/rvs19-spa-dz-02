#include "animation_matrix.hpp"

namespace anim = malaise::animation;

void anim::AnimationMatrix::push_current(const Animation &anim) {
	animations[animations.size() - 1].push(anim);
}

// Push to a newly created animation queue at the back of the vector
void anim::AnimationMatrix::push_and_create(const Animation &anim) {
	size_t index = animations.size();
	if (index >= animations.size()) animations.resize(index + 1);
	animations[index].push(anim);
}

// Push to a specific animation queue at an index
void anim::AnimationMatrix::push_to_index(const Animation &anim, const size_t index) {
	if (index >= animations.size()) return;
	animations[index].push(anim);
}

anim::AnimationQueue& anim::AnimationMatrix::at(const size_t index) {
	if (index >= animations.size()) throw std::out_of_range("AnimationQueue not initialized!");
	return animations[index];
}

std::vector<anim::AnimationQueue>& anim::AnimationMatrix::get_animations() {
	return animations;
}
