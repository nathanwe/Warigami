#ifndef GAME_START_SYSTEM_HPP
#define GAME_START_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <asset/scene_hydrater.hpp>
#include "components/board_square.hpp"
#include "components/board.hpp"
#include <glm/glm.hpp>
#include <stdio.h>


class game_start_system : public ecs::system_base
{
public:
	game_start_system(asset::scene_hydrater& _hydrater) : hydrater(_hydrater) {}

	
	void create_fire_graphic(glm::vec3 relitive_pos, entity_id parent)
	{
		ecs::entity nerd = hydrater.add_from_prototype("assets/prototypes/fire_graphic.json");
		transforms::transform& nerdT = nerd.get_component<transforms::transform>();
		nerdT.position = relitive_pos;
		nerdT.position.y += .6;
		nerdT.has_parent = true;
		nerdT.parent = parent;
		nerdT.is_matrix_dirty = true;
	}

	virtual void update(ecs::state& r_state) override
	{
		++updates;
		if (updates == 3) {
			r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, auto& board) {
				r_state.each_id<components::board_square, transforms::transform>([&](
					entity_id id,
					components::board_square& board_square,
					transforms::transform& transform)
					{
						if (id % 7 == 0) {
							board_square.terrain_type = terrain::fire;
							create_fire_graphic(transform.position, board_id);
						}
					});
				});
		}

	}

private:
	int updates = 0;
	asset::scene_hydrater& hydrater;
};

#endif
