#ifndef __SPIDERLING_SYSTEM_HPP_
#define __SPIDERLING_SYSTEM_HPP_

#include <asset/scene_hydrater.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <transforms/transform.hpp>

#include "components/game_piece.hpp"
#include "components/board.hpp"

int find_landing_zone(components::game_piece& game_piece, components::board& board);


class spiderling_system : public ecs::system_base
{
public:
	spiderling_system(asset::scene_hydrater& hydrater)
		: _hydrater(hydrater)
	{

	}

	void update(ecs::state& state)
	{
		state.each_id<transforms::transform, components::board>(
			[&](entity_id board_id, transforms::transform& board_t, components::board& board) {

				if (board.did_tick_elapse)
				{
					state.each<components::game_piece>([&](components::game_piece& game_piece)
						{
							spiderling_egg_spawns(state, game_piece, board);
							do_spawn_on_death_effect(state, game_piece, board);
						});
				}
			});
	}

private:
	asset::scene_hydrater& _hydrater;

	void spiderling_egg_spawns(ecs::state& r_state, components::game_piece& piece, components::board& board)
	{
		for (auto& effect : piece.effects)
		{
			if (
				effect == combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_FOR_HEALTH && 
				(piece.state == components::UNIT_STATE::ATTACK || piece.state == components::UNIT_STATE::MOVE)
			)
			{
				bool open_space[3] = { true, true, true };
				r_state.each<components::game_piece>([&](components::game_piece& game_piece)
					{
						if (game_piece.board_source == piece.board_source + glm::ivec2(0, 1 * piece.team)) 
						{
							open_space[0] = false;
						}

						if (game_piece.board_source == piece.board_source + glm::ivec2(1, 0) || piece.board_source.x >= board.columns-1)
						{
							open_space[1] = false;
						}

						if (game_piece.board_source == piece.board_source + glm::ivec2(-1, 0) || piece.board_source.x <= 0)
						{
							open_space[2] = false;
						}
					});

				if (open_space[0] && piece.spider_egg_spawn_counter == 0)
				{
					to_spawn new_spawn(piece.board_source.x, piece.board_source.y + (1 * piece.team), piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
					// Turn health spheres red
					if (piece.health_points.size() >= piece.health) {
						piece.health_points[piece.health].get_component<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(1, 0, 0);
					}
				}
				else if (open_space[1] && piece.spider_egg_spawn_counter == 2)
				{
					to_spawn new_spawn(piece.board_source.x + 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
					// Turn health spheres red
					if (piece.health_points.size() >= piece.health) {
						piece.health_points[piece.health].get_component<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(1, 0, 0);
					}
				}
				else if (open_space[2] && piece.spider_egg_spawn_counter == 1)
				{
					to_spawn new_spawn(piece.board_source.x - 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
					// Turn health spheres red
					if (piece.health_points.size() >= piece.health) {
						piece.health_points[piece.health].get_component<rendering::renderable_mesh_static>().material.param_diffuse = glm::vec3(1, 0, 0);
					}
				}

				piece.spider_egg_spawn_counter = (piece.spider_egg_spawn_counter + 1) % 2;
			}
		}
	}

	void do_spawn_on_death_effect(
		ecs::state r_state, 
		components::game_piece& game_piece,
		components::board& board)
	{
		if (game_piece.health <= 0 && (game_piece.state == components::UNIT_STATE::ATTACK || game_piece.state == components::UNIT_STATE::MOVE))
		{
			for (auto& effect : game_piece.effects)
			{
				switch (effect)
				{
				case combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_ON_DEATH:
				{
					to_spawn newSpawn(game_piece.board_destination.x, game_piece.board_destination.y, game_piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(newSpawn);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_SCISSOR_TROOPER_ON_DEATH:
				{
					to_spawn newSpawn2(game_piece.board_destination.x, game_piece.board_destination.y, game_piece.team, components::card_enum::SCISSOR_TROOPER);
					board.spawner.push_back(newSpawn2);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH:
				{
					to_spawn newSpawn3(game_piece.board_destination.x, game_piece.board_destination.y, game_piece.team * -1, components::card_enum::SCISSORLING);
					board.spawner.push_back(newSpawn3);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_TWO_DWARVES_ON_DEATH:
				{
					to_spawn newSpawn4(game_piece.board_destination.x, game_piece.board_destination.y, game_piece.team, components::card_enum::TWO_DWARVES);
					board.spawner.push_back(newSpawn4);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_ONE_DWARF_ON_DEATH:
				{
					to_spawn newSpawn5(game_piece.board_destination.x, game_piece.board_destination.y, game_piece.team, components::card_enum::ONE_DWARF);
					board.spawner.push_back(newSpawn5);
					break;
				}
				case combats::COMBAT_EFFECTS::CREATE_FIRE_TERRAIN_ON_DEATH:
				{
					r_state.each<components::terrain>([&](components::terrain& terrain)
						{
							if (terrain.location.x == game_piece.board_destination.x
								&& (terrain.location.y == game_piece.board_destination.y
									|| terrain.location.y == game_piece.board_destination.y + game_piece.team)) {
								terrain.type = components::TERRAIN_ENUM::FIRE;
								terrain.damage = 1;
								terrain.charges = -1;
								terrain.duration = 10;
								terrain.team = game_piece.team;
							}
						});
					break;
				}
				case combats::COMBAT_EFFECTS::YEET_SPIDERLING_ON_DEATH:
				{
					auto landing_zone = find_landing_zone(game_piece, board);
					to_spawn newSpawn(game_piece.board_destination.x, landing_zone, game_piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(newSpawn);
					break;
				}
				}
			}
		}
	}
};


int find_landing_zone(components::game_piece& game_piece, components::board& board)
{
	auto landing_zone = game_piece.board_destination.y;
	auto two_away = game_piece.board_destination.y + game_piece.team * 2;
	auto killed_by_ranged = board.board_state[game_piece.board_destination.x][landing_zone + game_piece.team] == 0;

	auto is_inbound = [&](int zone) {
		return zone >= 0 && zone < board.rows;
	};

	auto is_next_inbound = [&](int zone) {
		return zone + game_piece.team >= 0 &&
			zone + game_piece.team < board.rows;
	};

	auto landing_zone_free = [&]() {
		return board.board_state[game_piece.board_destination.x][landing_zone] == 0;
	};

	auto next_landing_zone_free = [&]() {
		return board.board_state[game_piece.board_destination.x][landing_zone + game_piece.team] == 0;
	};

	if (!is_inbound(two_away))
	{
		landing_zone = game_piece.board_destination.y;
	}
	else
	{
		landing_zone = two_away;

		while (is_next_inbound(landing_zone) && next_landing_zone_free())
			landing_zone += game_piece.team;
	}

	if (!is_next_inbound(landing_zone))
	{
		landing_zone = two_away;

		while (is_next_inbound(landing_zone) && !next_landing_zone_free())
			landing_zone += game_piece.team;
	}

	if (!is_inbound(landing_zone) && !landing_zone_free())
	{
		landing_zone = game_piece.board_destination.y;
	}

	return landing_zone;
}

#endif