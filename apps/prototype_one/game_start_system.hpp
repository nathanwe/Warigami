#ifndef GAME_START_SYSTEM_HPP
#define GAME_START_SYSTEM_HPP

#include <transforms/transform.hpp>
#include <asset/scene_hydrater.hpp>
#include "components/board_square.hpp"
#include "components/board.hpp"
#include <glm/glm.hpp>
#include <stdio.h>
#include "rendering/texture.hpp"
#include "rendering/asset_cache.hpp"
#include "components/terrain.hpp"

#include <string>


class game_start_system : public ecs::system_base
{
public:
	game_start_system(asset::scene_hydrater& _hydrater, rendering::asset_cache& asset_cache)
		: hydrater(_hydrater), _asset_cache(asset_cache) {}

	
	void create_fire_graphics(glm::vec3 relitive_pos, entity_id parent)
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
		if (updates == 1) {
			using namespace std::string_literals;
			//components::terrain::fire_texture_id = _asset_cache.get<rendering::texture>("assets/textures/terrain/12fire2.png").id;
			//components::terrain::fire_texture_id_p1 = _asset_cache.get<rendering::texture>("assets/textures/terrain/12fire2.png").id;
			//components::terrain::default_texture_id = _asset_cache.get<rendering::texture>("assets/textures/board_paper/wrinkled-paper-albedo.png").id;
			uint32_t f_id = _asset_cache.get<rendering::texture>("assets/textures/terrain/12fire2.png"s).id;
			uint32_t d_id = _asset_cache.get<rendering::texture>("assets/textures/board_paper/wrinkled-paper-albedo.png"s).id;
			uint32_t w_id = _asset_cache.get<rendering::texture>("assets/textures/terrain/web.png"s).id;
			r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, auto& board) {
				r_state.each_id<components::board_square, transforms::transform, components::terrain>([&](
					entity_id id,
					components::board_square& board_square,
					transforms::transform& transform,
					components::terrain& terrain)
					{
						terrain.location.x = board_square.x;
						terrain.location.y = board_square.y;
						terrain.fire_texture_id = f_id;
						terrain.default_texture_id = d_id;
						terrain.web_texture_id = w_id;
						
						if (board_square.x+2 > board_square.y) {
							board_square.team = 1.0f;
						}
						if (board_square.x+2 < board_square.y) {
							board_square.team = -1.0f;
						}
						
						/*
						if (id % 7 == 0) {
							terrain.type = components::TERRAIN_ENUM::FIRE;
							terrain.team = 0.0f;
							terrain.damage = 99;
							terrain.duration = -1;
						}
						if (id % 5 == 0) {
							terrain.type = components::TERRAIN_ENUM::WEB;
							terrain.team = 0.0f;
							terrain.damage = 99;
							terrain.charges = 5;
							terrain.duration = -1;
						}
						*/
					});
				});
		}

	}

private:
	int updates = 0;
	asset::scene_hydrater& hydrater;
	rendering::asset_cache& _asset_cache;
};

#endif
