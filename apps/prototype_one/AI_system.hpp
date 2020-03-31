#ifndef __AI_SYSTEM_HPP_
#define __AI_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <ecs/state.hpp>
#include <algorithm>

#include "components/board.hpp"
#include "components/pause.hpp"
#include "components/player.hpp" 


class AI_system : public ecs::system_base
{
public:
	AI_system(){}

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
					state.each<components::player>([&](components::player& player)
						{
							if (player.team == 1) {
								player.selected_row++;
								if (player.selected_row >= board.columns) {
									player.selected_row = 0;
								}
								place_card(1, player, state, board);
							}
						});
					
				}



			});

	}

private:
	//this should be a refrence to the version in player_controler, but i dont want to deal with statics and externs right now.
	void place_card(int loc, components::player& player, ecs::state& r_state, components::board& board) {
		if (loc != -1) {
			player.selected_card = player.hand[loc];
			player.selected_card_location = loc;
			bool taken = false;
			r_state.each<components::game_piece>([&](components::game_piece& piece)
				{
					if (piece.board_source == glm::ivec2(player.selected_row, player.score_column))
					{
						taken = true;
					}
				});

			if (!taken && player.energy >= components::card_costanamos[(int)player.hand[player.selected_card_location]])
			{
				board.spawner.emplace_back(
					player.selected_row,
					player.spawn_column,
					player.team,
					player.hand[player.selected_card_location]);

				player.energy -= components::card_costanamos[(int)player.hand[player.selected_card_location]];
				player.hand[player.selected_card_location] = player.safe_draw();
				player.state = components::PLAYER_STATE::BASE;
			}
		}
	}

};

#endif