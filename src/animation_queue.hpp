#ifndef MALAISE_ANIMATION_QUEUE_HPP
#define MALAISE_ANIMATION_QUEUE_HPP

#include <queue>

#include "animation.hpp"

namespace malaise::animation {

class AnimationQueue {
public:
	void push(const Animation &anim);
	void pop();
	Animation& front();

	bool empty() const;
private:
	std::queue<Animation> animation_queue{};
};

}

#endif // !MALAISE_ANIMATION_QUEUE_HPP
