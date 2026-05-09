#ifndef MALAISE_EVENT_MANAGER_HPP
#define MALAISE_EVENT_MANAGER_HPP

#include <vector>

#include "event.hpp"

namespace malaise::events {

class EventManager {
public:
	void push_event(const Event &event) {
		events.push_back(event);
	}

	void emplace_event(const float timer_,const std::function<void()> func_) {
		events.emplace_back(timer_, func_);
	}

	void update_and_run_events(const float delta_time) {
		for (auto it = events.begin(); it != events.end();) {
			it->update(delta_time);
			if (it->is_ready()) {
				it->run();
				it = events.erase(it); // fetch the next valid iterator
			} else {
				++it;
			}
		}
	}
private:
	std::vector<Event> events;
};

}

#endif // !MALAISE_EVENT_MANAGER_HPP
