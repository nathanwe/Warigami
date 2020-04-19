#ifndef __HEALTH_ORB_COLOR_SYSTEM_HPP_
#define __HEALTH_ORB_COLOR_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "combat.hpp"
#include "components/game_piece.hpp"


class health_orb_color_system : public ecs::system_base
{
public:
	health_orb_color_system()
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

			state.each<components::game_piece>([&]
			(components::game_piece& piece) {
						for (int i = 0; i < piece.max_health; i++) {
							glm::vec3 color = piece.health > i ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
							piece.health_points[i].get_component
								<rendering::renderable_mesh_static>().material.param_diffuse = color;
						}
				});
	}

private:

};

#endif