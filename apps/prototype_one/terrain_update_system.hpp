#ifndef __TERRAIN_UPDATE_SYSTEM_HPP_
#define __TERRAIN_UPDATE_SYSTEM_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <math.h>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/terrain.hpp"


class terrain_update_system : public ecs::system_base
{
public:
	terrain_update_system(core::frame_timer& timer, rendering::asset_cache& asset_cache)
		: _timer(timer), _asset_cache(asset_cache)
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
				state.each<components::terrain, rendering::renderable_mesh_static, components::board_square, transforms::transform>([&]
				(components::terrain& terrain, rendering::renderable_mesh_static& mesh, components::board_square& square, transforms::transform& square_t) {
						
					if (board.did_tick_elapse)
					{
						//terrain matenence
						if (terrain.duration > 0) {
							terrain.duration--;
						}
						if (terrain.duration == 0) {
							terrain.type = components::TERRAIN_ENUM::NONE;
							terrain.duration--;
						}
						if (terrain.charges == 0) {
							terrain.type = components::TERRAIN_ENUM::NONE;
							terrain.charges--;
						}
						if (terrain.growth_stage == 0) {
							terrain.type = components::TERRAIN_ENUM::NONE;
						}
						if (terrain.growth_stage > 3) {
							terrain.growth_stage--;
						}
						if (terrain.growth_stage == 1) {
							terrain.growth_stage--;
						}
						
						// terrain effects
						state.each<components::game_piece, transforms::transform>([&](auto& game_piece, auto& transform) {
							if (terrain.location == game_piece.board_destination)
							{								
								if (terrain.type == components::TERRAIN_ENUM::FIRE)
								{
									if (terrain.team != game_piece.team)
									{
										game_piece.health -= terrain.damage;	
										terrain.charges--;
									}
								}
								if (terrain.type == components::TERRAIN_ENUM::WEB)
								{
									if (terrain.team != game_piece.team)
									{
										game_piece.speed -= terrain.damage;
										terrain.charges--;
									}
								}
							}									
						});

					}					
					//terrain textures
					//type, team(-1,0,1), diffus normal or ambient,
					mesh.material.texture_diffuse = terrain.textures[(int)terrain.type][(int)terrain.team + 1][0][(int)terrain.growth_stage];
					mesh.material.texture_normal = terrain.textures[(int)terrain.type][(int)terrain.team + 1][1][(int)terrain.growth_stage];
					mesh.material.texture_ambient_occlusion = terrain.textures[(int)terrain.type][(int)terrain.team + 1][2][(int)terrain.growth_stage];

					});

				
			});
		
	}

private:
	core::frame_timer& _timer;
	rendering::asset_cache& _asset_cache;
};

#endif