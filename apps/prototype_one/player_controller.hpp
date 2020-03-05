#ifndef GAME_PLAYER_CONTROLLER_HPP
#define GAME_PLAYER_CONTROLLER_HPP

#define ROUND_TIME 2.0f

#include "ecs/state.hpp"
#include <core/game_input_manager.hpp>
#include <core/frame_timer.hpp>
#include <asset/scene_hydrater.hpp>
#include <event/event_manager.hpp>
#include <transforms/transform.hpp>

#include "components/player.hpp" 
#include "components/board_square.hpp"
#include "components/game_piece.hpp"

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

	components::card_enum card_select(components::player& player, components::PLAYER_STATE* playerState, int num)
	{
		if (components::card_costanamos[(int)player.hand[num]] > player.energy)
		{
			*playerState = components::PLAYER_STATE::BASE;
		}
		else
		{
			*playerState = components::PLAYER_STATE::UNIT_PLACEMENT;
		}
		return player.hand[num];
	}

	void row_select(transforms::transform& transform, float num)
	{
		transform.scale.y = num;
		transform.is_matrix_dirty = true;
	}

	void spawn_unit(int lane, int team, ecs::state& r_state, components::card_enum type)
	{
		static const std::string CardPrototypes[(size_t)components::card_enum::TOTAL_CARDS] = {
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
		bool add_energy = false;
		if (timer <= 0)
		{
			timer = ROUND_TIME;
			add_energy = true;
		}
		else
		{
			timer -= m_timer.smoothed_delta_secs() * 10.f;
		}
		r_state.each<components::player>([&](components::player& player)
		{
			core::controls card1;
			core::controls card2;
			core::controls card3;
			core::controls card4;
			int column_to_up_scale;
			float forward;
			
			if (add_energy && player.energy < 10)
				player.energy++;

			std::cerr << "Player " << player.team << " energy: " << player.energy << std::endl;
            std::cerr << "Player " << player.team << " helath: " << player.health << std::endl;

			if (player.team == 1.0f) {
				card1 = core::CARD1_CONTROL;
				card2 = core::CARD2_CONTROL;
				card3 = core::CARD3_CONTROL;
				card4 = core::CARD4_CONTROL;
				column_to_up_scale = 0;
				forward = m_input.forward();
			}
			else if (player.team == -1.0f) {
				card1 = core::CARD1_CONTROL_PLAYER2;
				card2 = core::CARD2_CONTROL_PLAYER2;
				card3 = core::CARD3_CONTROL_PLAYER2;
				card4 = core::CARD4_CONTROL_PLAYER2;
				column_to_up_scale = 8;
				forward = m_input.forward_player2();
			}
			
			
			if (player.state == components::PLAYER_STATE::BASE)
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
					player.selected_card = card_select(player, &(player.state), loc);
					player.selected_card_location = loc;
				}
			}
			else if (player.state == components::PLAYER_STATE::UNIT_PLACEMENT)
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
						spawn_unit(player.selected_row, player.team, r_state, player.hand[player.selected_card_location]);
						player.energy -= components::card_costanamos[(int)player.hand[player.selected_card_location]];
						player.hand[player.selected_card_location] = player.safe_draw();
					}
						
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{
						transform.scale.y = 1;
						transform.is_matrix_dirty = true;
					});

					player.state = components::PLAYER_STATE::BASE;
				}
				else if (m_input.is_input_started(card2))
				{
					player.state = components::PLAYER_STATE::BASE;
				}
				else if (forward > .4f)
				{
					if (player.selected_row > 0 && player.row_select_delay <= 0.f)
					{
						player.row_select_delay = 0.1f;
						player.selected_row--;
					}
					else
					{
						player.row_select_delay -= m_timer.smoothed_delta_secs();
					}
				}
				else if (forward < -.4f)
				{
					if (player.selected_row < 6 && player.row_select_delay <= 0.f)
					{
						player.row_select_delay = 0.1f;
						player.selected_row++;
					}
					else
					{
						player.row_select_delay -= m_timer.smoothed_delta_secs();
					}
				}
				
			}
			else if (player.state == components::PLAYER_STATE::DICE_PLACEMENT)
			{
				//WIP
			}
		});
	}

private:
	float timer = ROUND_TIME;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;
};

#endif
