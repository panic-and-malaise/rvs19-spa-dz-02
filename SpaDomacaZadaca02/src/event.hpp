#ifndef MALAISE_EVENT_HPP
#define MALAISE_EVENT_HPP

#include <functional>

namespace malaise::events {

enum class Priority {
	MINIMUM = 1,
	LOW,
	NORMAL,
	HIGH,
	MAXIMUM
};

class Event {
public:
	Event( const float timer_,const std::function<void()> func_, const Priority priority_ = Priority::NORMAL)
		: timer(timer_), event_func(func_), priority(priority_) {}

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
	Priority priority = Priority::NORMAL;

	friend class EventCompare;
};

struct EventCompare { // For priority_queue, since that sounds like it would be neat with this
	bool operator()(const Event &_Left, const Event &_Right) {
		return static_cast<int>(_Left.priority) < static_cast<int>(_Right.priority);
	}
};

};

#endif // !MALAISE_EVENT_HPP
