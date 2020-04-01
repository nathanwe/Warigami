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
			//type, team(-1,0,1), diffus normal or ambient,
			uint32_t textures[(int)components::TERRAIN_ENUM::NUM][3][3];
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					for (int k = 0; k < (int)components::TERRAIN_ENUM::NUM; k++) {
						textures[k][i][j] = 0;
					}
				}
				textures[(int)components::TERRAIN_ENUM::NONE][i][0] = _asset_cache.get<rendering::texture>("assets/textures/board_paper/wrinkled-paper-albedo.png"s).id;
				textures[(int)components::TERRAIN_ENUM::NONE][i][1] = _asset_cache.get<rendering::texture>("assets/textures/board_paper/wrinkled-paper-normal-ogl.png"s).id;
				textures[(int)components::TERRAIN_ENUM::NONE][i][2] = _asset_cache.get<rendering::texture>("assets/textures/board_paper/wrinkled-paper-ao.png"s).id;
			}
			textures[(int)components::TERRAIN_ENUM::FIRE][0][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/fire2.png"s).id;
			textures[(int)components::TERRAIN_ENUM::FIRE][1][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/12fire2.png"s).id;
			textures[(int)components::TERRAIN_ENUM::FIRE][2][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/fire.png"s).id;
			
			textures[(int)components::TERRAIN_ENUM::WEB][0][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/web2.png"s).id;
			textures[(int)components::TERRAIN_ENUM::WEB][1][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/web.png"s).id;
			textures[(int)components::TERRAIN_ENUM::WEB][2][0] = _asset_cache.get<rendering::texture>("assets/textures/terrain/web1.png"s).id;

			r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, auto& board) {
				r_state.each_id<components::board_square, transforms::transform, components::terrain>([&](
					entity_id id,
					components::board_square& board_square,
					transforms::transform& transform,
					components::terrain& terrain)
					{

						terrain.location.x = board_square.x;
						terrain.location.y = board_square.y;

						for (int i = 0; i < 3; i++) {
							for (int j = 0; j < 3; j++) {
								for (int k = 0; k < (int)components::TERRAIN_ENUM::NUM; k++) {
									terrain.textures[k][i][j] = textures[k][i][j];
								}
							}
						}
										
						if (board_square.x+2 > board_square.y) {
							board_square.team = 1.0f;
						}
						if (board_square.x+2 < board_square.y) {
							board_square.team = -1.0f;
						}						
						
						/*
						if (id % 11 == 0) {
							terrain.type = components::TERRAIN_ENUM::FIRE;
							terrain.team = -1.0f;
							terrain.damage = 99;
							terrain.duration = -1;
						}
						if (id % 11 == 1) {
							terrain.type = components::TERRAIN_ENUM::FIRE;
							terrain.team = 0.0f;
							terrain.damage = 99;
							terrain.duration = -1;
						}
						if (id % 11 == 2) {
							terrain.type = components::TERRAIN_ENUM::FIRE;
							terrain.team = 1.0f;
							terrain.damage = 99;
							terrain.duration = -1;
						}
						if (id % 11 == 5) {
							terrain.type = components::TERRAIN_ENUM::WEB;
							terrain.team = -1.0f;
							terrain.damage = 99;
							terrain.charges = 5;
							terrain.duration = -1;
						}
						if (id % 11 == 6) {
							terrain.type = components::TERRAIN_ENUM::WEB;
							terrain.team = 0.0f;
							terrain.damage = 99;
							terrain.charges = 5;
							terrain.duration = -1;
						}
						if (id % 11 == 7) {
							terrain.type = components::TERRAIN_ENUM::WEB;
							terrain.team = 1.0f;
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
