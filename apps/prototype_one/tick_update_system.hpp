#ifndef __TICK_UPDATE_SYSTEM_HPP_
#define __TICK_UPDATE_SYSTEM_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>

#include "components/board.hpp"
#include "components/pause.hpp"

class tick_update_system : public ecs::system_base
{
public:
	tick_update_system(core::frame_timer& timer)
		: _timer(timer)
	{}

	void update(ecs::state& state)
	{
		// Skip system when paused
		auto e = state.first<components::pause>([](auto& pause)
		{
			return true;
		});
		if (e)
		{
			auto& pause = e->get_component<components::pause>();
			if (pause.is_game_paused || !pause.is_game_started || pause.is_game_over)
			{
				return;
			}
		}

		state.each<components::board>([&](components::board& board) {
			if (board.state != components::game_state::gameplay) return;

			board.tick_time_remaining -= _timer.smoothed_delta_secs();
			
			if (board.tick_time_remaining <= 0.f)
			{
				board.tick_time_remaining = board.tick_time;
				board.did_tick_elapse = true;
			}
			else
			{
				board.did_tick_elapse = false;
			}

			board.timer_t = (board.tick_time - board.tick_time_remaining) / board.tick_time;
			board.ticker_dt = _timer.smoothed_delta_secs() / board.tick_time;
		});
	}

private:
	core::frame_timer& _timer;

};

#endif