#ifndef MALAISE_EVENT_HPP
#define MALAISE_EVENT_HPP

#include <functional>

namespace malaise::events {

class Event {
public:
	Event( const float timer_,const std::function<void()> func_)
		: timer(timer_), event_func(func_) {}

	void run(void) {
		event_func();
	}

	void update(const float delta_time) {
		timer -= delta_time;
	}

	bool is_ready(void) const {
		return timer <= 0.f;
	}
private:
	float timer = 0.0f;
	std::function<void()> event_func;

	friend class EventCompare;
};

};

#endif // !MALAISE_EVENT_HPP
