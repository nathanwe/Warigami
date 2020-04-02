#ifndef __SPIDERLING_SYSTEM_HPP_
#define __SPIDERLING_SYSTEM_HPP_

#include <asset/scene_hydrater.hpp>
#include <ecs/state.hpp>
#include <ecs/system_base.hpp>
#include <transforms/transform.hpp>

#include "components/game_piece.hpp"
#include "components/board.hpp"


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
	;

	void spiderling_egg_spawns(ecs::state& r_state, components::game_piece& piece, components::board& board)
	{
		for (auto& effect : piece.effects)
		{
			if (effect == combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_FOR_HEALTH)
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

				if (open_space[0])
				{
					to_spawn new_spawn(piece.board_source.x, piece.board_source.y + (1 * piece.team), piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
				}
				else if (open_space[1])
				{
					to_spawn new_spawn(piece.board_source.x + 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
				}
				else if (open_space[2])
				{
					to_spawn new_spawn(piece.board_source.x - 1, piece.board_source.y, piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(new_spawn);
					piece.health -= 1;
				}
			}
		}
	}

	void do_spawn_on_death_effect(ecs::state r_state, components::game_piece& game_piece,
		components::board& board)
	{
		if (game_piece.health <= 0)
		{
			for (auto& effect : game_piece.effects)
			{
				switch (effect)
				{
				case combats::COMBAT_EFFECTS::SPAWN_SCISSORLING_ON_DEATH:
				{
					to_spawn newSpawn(game_piece.board_source.x, game_piece.board_source.y, game_piece.team, components::card_enum::SCISSORLING);
					board.spawner.push_back(newSpawn);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_SCISSOR_TROOPER_ON_DEATH:
				{
					to_spawn newSpawn2(game_piece.board_source.x, game_piece.board_source.y, game_piece.team, components::card_enum::SCISSOR_TROOPER);
					board.spawner.push_back(newSpawn2);
					break;
				}
				case combats::COMBAT_EFFECTS::SPAWN_ENEMY_SPIDERLING_ON_DEATH:
				{
					to_spawn newSpawn3(game_piece.board_source.x, game_piece.board_source.y, game_piece.team * -1, components::card_enum::SCISSORLING);
					board.spawner.push_back(newSpawn3);
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
				}
			}
		}
	}
};


#endif