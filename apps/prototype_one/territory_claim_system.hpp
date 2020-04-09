#ifndef __TERRITORY_CLAIM_SYSTEM_HPP_
#define __TERRITORY_CLAIM_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "combat.hpp"
#include "components/game_piece.hpp"


class territory_claim_system : public ecs::system_base
{
public:
	territory_claim_system()
	{}

	void update(ecs::state& state)
	{
		// Skip system when paused
		auto e = state.first<components::pause>([](auto& pause)
			{
				return true;
			});
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) {


				if (board.did_tick_elapse)
				{
					state.each<components::board_square>(
						[&](components::board_square& board_square) {
							board_square.team = board_square.next_team;							
						});
				}
			});		

	}

private:

};

#endif