#ifndef GAME_PLAYER_CONTROLLER_HPP
#define GAME_PLAYER_CONTROLLER_HPP

#include "ecs/state.hpp"
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <asset/scene_hydrater.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

#include "components/player.hpp" 
#include "components/board_square.hpp"
#include "components/game_piece.hpp"

namespace game {
	enum class PLAYER_STATE
	{
		BASE,
		PLACEMENT
	};
}
class player_controller : public ecs::system_base, public event::Listener
{
public:

	player_controller(core::game_input_manager& input, core::frame_timer& timer,
		event::EventManager& _event_manager, asset::scene_hydrater& _hydrater) :
		m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
	{

	}

	void HandleEvent(event::Event& event)
	{

	}

	components::card_enum card_select(components::player& player, game::PLAYER_STATE* playerState, int num)
	{
		*playerState = game::PLAYER_STATE::PLACEMENT;
		return player.hand[num];
	}

	void row_select(transforms::transform& transform, float num)
	{
		transform.scale.y = num;
		transform.is_matrix_dirty = true;
	}

	void spawn_unit(int lane, int team, ecs::state& r_state, components::card_enum type)
	{
		static const std::string CardPrototypes[(size_t)components::card_enum::TOTA_CARDS] = {
			"assets/prototypes/basic_unit.json",
			"assets/prototypes/basic_unit.json",
			"assets/prototypes/ranged_unit.json",
			"assets/prototypes/fast_unit.json"
		};
	
		size_t type_index = (size_t)type;		
		ecs::entity& nerd = hydrater.add_from_prototype(CardPrototypes[type_index]);
		transforms::transform& nerdT = nerd.get_component<transforms::transform>();
		components::game_piece& nerdP = nerd.get_component<components::game_piece>();
		
		nerdP.team = team;
		nerdP.board_source.x = lane;
		nerdP.board_source.y = nerdP.team >= 0 ? 0 : 8;
		
		if (nerdP.team >= 0)
			nerdT.rotation.y = glm::pi<float>() - glm::half_pi<float>()/2.f;
		else
			nerdT.rotation.y = + glm::half_pi<float>()/2.f;

		nerdP.continuous_board_location = nerdP.board_source;
		nerdP.board_destination = nerdP.board_source;
		nerdP.move_board = nerdP.move_board * nerdP.team;
		nerdP.move_world = nerdP.move_world * (float)nerdP.team;

		std::vector<glm::ivec2> new_attacks;
		for (int i = 0; i < nerdP.attacks.size(); i++)
		{
			new_attacks.push_back(nerdP.attacks[i] * nerdP.team);
		}
		nerdP.attacks = new_attacks;

		nerdT.scale = glm::vec3(0.5);

		r_state.each_id<components::board, transforms::transform>([&](entity_id id, components::board& board, transforms::transform& transform)
		{
			nerdT.has_parent = true;
			nerdT.parent = id;
		});
	}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each<components::player>([&](components::player& player)
		{
			game::PLAYER_STATE* state_px_p;
			core::controls card1;
			core::controls card2;
			core::controls card3;
			core::controls card4;
			components::card_enum* selected_card_px_p;
			int* selected_card_location_px_p;
			float* row_select_delay_px_p;
			int column_to_up_scale;

			if (player.team == 1.0f) {
				state_px_p = &state_p1;
				card1 = core::CARD1_CONTROL;
				card2 = core::CARD2_CONTROL;
				card3 = core::CARD3_CONTROL;
				card4 = core::CARD4_CONTROL;
				selected_card_px_p = &selected_card_p1;
				selected_card_location_px_p = &selected_card_location_p1;
				row_select_delay_px_p = &row_select_delay;
				column_to_up_scale = 0;
			}
			else if (player.team == -1.0f) {
				state_px_p = &state_p2;
				card1 = core::CARD1_CONTROL_PLAYER2;
				card2 = core::CARD2_CONTROL_PLAYER2;
				card3 = core::CARD3_CONTROL_PLAYER2;
				card4 = core::CARD4_CONTROL_PLAYER2;
				selected_card_px_p = &selected_card_p2;
				selected_card_location_px_p = &selected_card_location_p2;
				row_select_delay_px_p = &row_select_delay_two;
				column_to_up_scale = 8;
			}
			
			
			if (*state_px_p == game::PLAYER_STATE::BASE)
			{	
				int loc = -1;
				if (m_input.is_input_started(card1))
				{
					loc = 0;
				}
				else if (m_input.is_input_started(card2))
				{
					loc = 1;
				}
				else if (m_input.is_input_started(card3))
				{
					loc = 2;
				}
				else if (m_input.is_input_started(card4))
				{
					loc = 3;
				}
				if (loc != -1) {
					*selected_card_px_p = card_select(player, &(*state_px_p), loc);
					*selected_card_location_px_p = loc;
				}
			}
			else if (*state_px_p == game::PLAYER_STATE::PLACEMENT)
			{
				r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
				{
					if (square.y == column_to_up_scale)
					{
						square.x == player.selected_row ? row_select(transform, 1.2f) : row_select(transform, 1);
					}
				});

				if (m_input.is_input_started(card1))
				{
					bool taken = false;
					r_state.each<components::game_piece>([&](components::game_piece& piece)
					{
						if (piece.board_source == glm::ivec2(player.selected_row, column_to_up_scale))
						{
							taken = true;
						}
					});

					if (!taken)
					{
						spawn_unit(player.selected_row, player.team, r_state, player.hand[*selected_card_location_px_p]);
						player.hand[*selected_card_location_px_p] = player.safe_draw();
					}
						
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{
						transform.scale.y = 1;
						transform.is_matrix_dirty = true;
					});

					*state_px_p = game::PLAYER_STATE::BASE;
				}
				else if (m_input.is_input_started(card2))
				{
					*state_px_p = game::PLAYER_STATE::BASE;
				}
				else if (m_input.forward() > .4f)
				{
					if (player.selected_row > 0 && *row_select_delay_px_p <= 0.f)
					{
						*row_select_delay_px_p = 0.1f;
						player.selected_row--;
					}
					else
					{
						*row_select_delay_px_p -= m_timer.smoothed_delta_secs();
					}
				}
				else if (m_input.forward() < -.4f)
				{
					if (player.selected_row < 6 && *row_select_delay_px_p <= 0.f)
					{
						*row_select_delay_px_p = 0.1f;
						player.selected_row++;
					}
					else
					{
						*row_select_delay_px_p -= m_timer.smoothed_delta_secs();
					}
				}
				
			} 
			/*else if (player.team == -1.0f) {
				if (state_p2 == game::PLAYER_STATE::BASE)
				{
					if (m_input.is_input_started(core::CARD1_CONTROL_PLAYER2))
					{
						selected_card_p2 = card_select(player, &state_p2, 0);
						selected_card_location_p2 = 0;
					}
					else if (m_input.is_input_started(core::CARD2_CONTROL_PLAYER2))
					{
						selected_card_p2 = card_select(player, &state_p2, 1);
						selected_card_location_p2 = 1;
					}
					else if (m_input.is_input_started(core::CARD3_CONTROL_PLAYER2))
					{
						selected_card_p2 = card_select(player, &state_p2, 2);
						selected_card_location_p2 = 2;
					}
					else if (m_input.is_input_started(core::CARD4_CONTROL_PLAYER2))
					{
						selected_card_p2 = card_select(player, &state_p2, 3);
						selected_card_location_p2 = 3;
					}
				}
				else if (state_p2 == game::PLAYER_STATE::PLACEMENT)
				{
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{
						if (square.y == 8)
						{
							square.x == player.selected_row ? row_select(transform, 1.2f) : row_select(transform, 1);
						}
					});

					if (m_input.is_input_started(core::CARD1_CONTROL_PLAYER2))
					{
						bool taken = false;
						r_state.each<components::game_piece>([&](components::game_piece& piece)
						{
							if (piece.board_source == glm::ivec2(player.selected_row, 0))
							{
								taken = true;
							}
						});

						if (!taken)
						{
							spawn_unit(player.selected_row, player.team, r_state, player.hand[selected_card_location_p2]);
							player.hand[selected_card_location_p2] = player.safe_draw();
						}
						r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
						{
							transform.scale.y = 1;
							transform.is_matrix_dirty = true;
						});

						state_p2 = game::PLAYER_STATE::BASE;
					}
					else if (m_input.is_input_started(core::CARD2_CONTROL_PLAYER2))
					{
						state_p2 = game::PLAYER_STATE::BASE;
					}
					else if (m_input.forward_player2() > .4f)
					{
						if (player.selected_row > 0 && row_select_delay_two <= 0.f)
						{
							row_select_delay_two = 0.1f;
							player.selected_row--;
						}
						else
						{
							row_select_delay_two -= m_timer.smoothed_delta_secs();
						}
					}
					else if (m_input.forward_player2() < -.4f)
					{
						if (player.selected_row < 6 && row_select_delay_two <= 0.f)
						{
							row_select_delay_two = 0.1f;
							player.selected_row++;
						}
						else
						{
							row_select_delay_two -= m_timer.smoothed_delta_secs();
						}
					}
				}
			}*/
		});
	}

private:
	components::card_enum selected_card_p1;
	int selected_card_location_p1;
	components::card_enum selected_card_p2;
	int selected_card_location_p2;
	float row_select_delay = .1f;
	float row_select_delay_two = .1f;
	game::PLAYER_STATE state_p1 = game::PLAYER_STATE::BASE;
	game::PLAYER_STATE state_p2 = game::PLAYER_STATE::BASE;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;
};

#endif
