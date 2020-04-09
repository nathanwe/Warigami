#ifndef __HEALTH_REGENERATION_SYSTEM_HPP_
#define __HEALTH_REGENERATION_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "combat.hpp"
#include "components/game_piece.hpp"


class health_regenration_system : public ecs::system_base
{
public:
	health_regenration_system()
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
								if (piece.health > 0) {
									int health_to_add = std::count(piece.effects.begin(),
										piece.effects.end(), combats::COMBAT_EFFECTS::REGENERATE);
									health_to_add = std::min(health_to_add, piece.max_health - piece.health);
									if (health_to_add > 0) {
										for (int i = 0; i < health_to_add; i++) {

											piece.health_points[piece.health].get_component
												<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(0, 1, 0);
											piece.health++;
										}
									}
								}
							});
					}
						
					

			});

	}

private:

};

#endif