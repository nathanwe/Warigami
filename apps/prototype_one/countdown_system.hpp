#ifndef WIZARD_GAME_COUNTDOWN_SYSTEM_HPP
#define WIZARD_GAME_COUNTDOWN_SYSTEM_HPP

#include <ecs/system_base.hpp>
#include <event/event_manager.hpp>
#include <core/frame_timer.hpp>

#include "components/countdown.hpp"

class countdown_system : public ecs::system_base
{
public:
	countdown_system(
		core::frame_timer& timer) : m_timer(timer) {}

	void update(ecs::state& r_state) override
	{
		if (is_first_run)
		{
			r_state.each<components::countdown>([&](auto& countdown)
			{
				countdown.current_value = countdown.count_duration;
				std::cerr << countdown.current_value << std::endl;
			});
			is_first_run = false;
		}
		else if (!ended)
		{
			r_state.each<components::countdown>([&](auto& countdown)
			{
				countdown.current_value -= m_timer.smoothed_delta_secs();
				if (countdown.current_value <= 0) {
					ended = true;
					// Create event to select winner, restart game, etc
				}
			});
		}
	}

private:
	core::frame_timer& m_timer;
	bool is_first_run = true;
	bool ended = false;
};

#endif