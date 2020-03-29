#ifndef __HEALTH_REGENERATION_SYSTEM_HPP_
#define __HEALTH_REGENERATION_SYSTEM_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/terrain.hpp"


class health_regeration_system : public ecs::system_base
{
public:
	health_regeneration_system(core::frame_timer& timer)
		: _timer(timer)
	{}

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
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) {
				

					if (board.did_tick_elapse)
					{
						state.each<components::game_piece>([&]
						(components::game_piece& piece) {

							});
					}
						
					

			});

	}

private:
	core::frame_timer& _timer;

};

#endif