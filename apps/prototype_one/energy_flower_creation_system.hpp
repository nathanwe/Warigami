#ifndef __ENERGY_FLOWER_CREATION_SYSTEM_HPP_
#define __ENERGY_FLOWER_CREATION_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <algorithm>
#include <random>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "combat.hpp"
#include "components/game_piece.hpp"
#include "components/terrain.hpp"


class energy_flower_creation_system : public ecs::system_base
{
public:
	energy_flower_creation_system()
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
					std::random_device rd;
					auto rng = std::default_random_engine{ rd() };
					std::uniform_int_distribution<int> dice(0, 70);
					auto d99 = std::bind(dice, rng);

					state.each_id<components::board_square, components::terrain>([&]
					(entity_id board_square_id, components::board_square& square, components::terrain& terrain) {
							if (d99() == 1 && terrain.type == components::TERRAIN_ENUM::NONE) {
								terrain.type = components::TERRAIN_ENUM::ENERGY_FLOWER;
								terrain.team = 0.0f;
								terrain.damage = -1;
								terrain.duration = -1;
								terrain.growth_stage = 7;
							}


						});
				}



			});

	}

private:

};

#endif