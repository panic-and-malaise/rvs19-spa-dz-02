#include "animation_queue.hpp"

namespace anim = malaise::animation;

void anim::AnimationQueue::push(const Animation &anim) {
	animation_queue.push(anim);
}

void anim::AnimationQueue::pop() {
	animation_queue.pop();
}

anim::Animation& anim::AnimationQueue::front() {
	return animation_queue.front();
}

bool anim::AnimationQueue::empty() const {
	return animation_queue.empty();
}
