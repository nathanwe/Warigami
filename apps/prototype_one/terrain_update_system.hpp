#ifndef __TERRAIN_UPDATE_SYSTEM_HPP_
#define __TERRAIN_UPDATE_SYSTEM_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <transforms/transform.hpp>
#include <rendering/renderable_mesh_static.hpp>

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
				state.each<components::terrain, rendering::renderable_mesh_static, components::board_square>([&]
				(components::terrain& terrain, rendering::renderable_mesh_static& mesh, components::board_square& square) {
						
						if (board.did_tick_elapse)
						{
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
							// If a unit is standing in fire, it takes damage;
							state.each<components::game_piece, transforms::transform>([&](auto& game_piece, auto& transform) {
								if (terrain.location == game_piece.board_destination)
								{
									if (terrain.type == components::TERRAIN_ENUM::FIRE)
									{
										if (terrain.team != game_piece.team)
										{
											game_piece.health -= terrain.damage;											
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
						terrain_upkeep(terrain, mesh, square);
					});
				
			});
		
	}

private:
	core::frame_timer& _timer;
	rendering::asset_cache& _asset_cache;
	void terrain_upkeep(components::terrain& terrain,
		rendering::renderable_mesh_static& mesh, components::board_square& square) {				
				switch (terrain.type)
				{
				case components::TERRAIN_ENUM::NONE:
				{

					mesh.material.texture_diffuse = terrain.default_texture_id;
					//mesh.material.texture_normal = (uint32_t)"assets/textures/board_paper/wrinkled-paper-normal-ogl.png";
					//mesh.material.texture_ambient_occlusion = (uint32_t)"assets/textures/board_paper/wrinkled-paper-ao.png";
					break;
				}
				case components::TERRAIN_ENUM::FIRE:
				{
					mesh.material.texture_diffuse = terrain.fire_texture_id;
					if (terrain.team == 1.0f) {
						
						//mesh.material.texture_diffuse = components::terrain::fire_texture_id_p1;
					}
					else if (terrain.team == -1.0f) {
						//mesh.material.texture_diffuse = components::terrain::fire_texture_id_p2;
					}
					else {
						//mesh.material.texture_diffuse = components::terrain::fire_texture_id;
					}
					break;
				}
				case components::TERRAIN_ENUM::WEB:
				{
					mesh.material.texture_diffuse = terrain.web_texture_id;
					if (terrain.team == 1.0f) {
						
						//mesh.material.texture_diffuse = (uint32_t)"assets/textures/terrain/web1.png";
					}
					else if (terrain.team == -1.0f) {
						//mesh.material.texture_diffuse = (uint32_t)"assets/textures/terrain/web2.png";
					}
					else {
						//mesh.material.texture_diffuse = (uint32_t)"assets/textures/terrain/web.png";
					}
					break;
				}
				default:
					break;
				}
			
	}

};

#endif