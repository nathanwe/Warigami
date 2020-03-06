#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>

#include "components/countdown.hpp"

class game_over_event : public event::Event
{
public:
	explicit game_over_event() : event::Event(event::EVENT_TYPE::GAME_OVER)
	{}
};

class countdown_system : public ecs::system_base
{
public:
	countdown_system(
		core::frame_timer& timer, 
		event::EventManager& _event_manager) 
		: m_timer(timer),
		event_manager(_event_manager) {}

	void update(ecs::state& r_state) override
	{
		if (is_first_run)
		{
			r_state.each<components::countdown>([&](auto& countdown)
			{
				countdown.current_value = countdown.count_duration;
			});
			is_first_run = false;
		}
		else if (!ended)
		{
			r_state.each<components::countdown>([&](auto& countdown)
			{
				countdown.current_value -= m_timer.delta_secs();
				if (countdown.current_value <= 0) {
					countdown.current_value = countdown.count_duration;
					ended = true;
					game_over_event go;
					event_manager.BroadcastEvent(go);
					std::cerr << "Game Over!" << std::endl;
				}
			});
		}
	}

private:
	core::frame_timer& m_timer;
	bool is_first_run = true;
	bool ended = false;
	event::EventManager& event_manager;
};

#endif