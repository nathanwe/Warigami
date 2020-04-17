#ifndef __AI_SYSTEM_HPP_
#define __AI_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/player.hpp" 
#include "player_controller.hpp"


class AI_system : public ecs::system_base, public event::Listener
{
public:
	AI_system(core::frame_timer& timer, asset::scene_hydrater& hydrater, event::EventManager& _event_manager)
		: _timer(timer)
		, _hydrater(hydrater)
		, event_manager(_event_manager)
	{
		event_manager.Subscribe(this, event::EVENT_TYPE::TOGGLE_P1_AI);
		event_manager.Subscribe(this, event::EVENT_TYPE::TOGGLE_P2_AI);
		event_manager.Subscribe(this, event::EVENT_TYPE::ATTRACT_MODE);
	}

	void HandleEvent(event::Event& event)
	{
		if (event.mType == event::EVENT_TYPE::TOGGLE_P1_AI)
		{
			std::cerr << "Event recieved" << std::endl;			
		}
	}

	void update(ecs::state& state)
	{
		// Skip system when paused
		auto e = state.first<components::pause>([](auto& pause)
			{
				return true;
			});
		if (e && e->get_component<components::pause>().is_game_paused)
		{
			return;
		}
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) 
			{				
				state.each<components::player>([&](components::player& player)
					{
						if (player.controlled_by_AI)
						{
							player.pull = true;
							if (board.did_tick_elapse)
							{
								player.pull = false;
								
								if (player.selected_row >= board.columns - 1) {
									player.AI_movement_direction = -1;
								}
								if (player.selected_row <= 0) {
									player.AI_movement_direction = 1;
								}
								player.selected_row += player.AI_movement_direction;
								if (player.selected_row != board.columns - 1) {
									player.place_card(0, _timer.total_s(), state, board.spawner, _hydrater);
							}
						}
					}
				});					
			});

	}

private:
	core::frame_timer& _timer;
	asset::scene_hydrater& _hydrater;
	event::EventManager& event_manager;
};

#endif