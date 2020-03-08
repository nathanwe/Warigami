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

struct player_controls
{
    core::controls card1;
    core::controls card2;
    core::controls card3;
    core::controls card4;
    core::controls dice_button;
    int column_to_up_scale;
};

class player_controller : public ecs::system_base, public event::Listener
{
public:

	player_controller(core::game_input_manager& input, core::frame_timer& timer,
		event::EventManager& _event_manager, asset::scene_hydrater& _hydrater) :
		m_input(input), m_timer(timer), event_manager(_event_manager), hydrater(_hydrater)
	{

	}

	void HandleEvent(event::Event& event) override
	{

	}

	static components::card_enum card_select(components::player& player, components::PLAYER_STATE* playerState, int num)
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

	static void row_select(transforms::transform& transform, float num)
	{
		transform.scale.y = num;
		transform.is_matrix_dirty = true;
	}

	void spawn_unit(int lane, int team, ecs::state& r_state, components::card_enum type)
	{
		static const std::string CardPrototypes[(size_t)components::card_enum::TOTAL_CARDS] = {
			"assets/prototypes/scissorling.json",
			"assets/prototypes/scissorling_twin.json",
			"assets/prototypes/scissor_trooper.json",
			"assets/prototypes/scissorling_egg.json",
			"assets/prototypes/scissor_webber.json",
			"assets/prototypes/scissor_goliath.json",
			"assets/prototypes/scissor_queen.json",
			"assets/prototypes/scissor_titan.json"
		};
	
		auto type_index = (size_t)type;
		ecs::entity& nerd = hydrater.add_from_prototype(CardPrototypes[type_index]);
		auto& nerdT = nerd.get_component<transforms::transform>();
		auto& nerdP = nerd.get_component<components::game_piece>();
		
		nerdP.team = team;
		nerdP.board_source.x = lane;
		nerdP.board_source.y = nerdP.team >= 0 ? 0 : 8;
		
		if (nerdP.team >= 0)
			nerdT.rotation.y = glm::pi<float>() - glm::quarter_pi<float>();
		else
			nerdT.rotation.y = glm::quarter_pi<float>();

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
	void create_fire_graphic(glm::vec3 relitive_pos, entity_id parent)
	{
		ecs::entity nerd = hydrater.add_from_prototype("assets/prototypes/fire_graphic.json");
		auto& nerdT = nerd.get_component<transforms::transform>();
		nerdT.position = relitive_pos;
		nerdT.position.y += .6;
		nerdT.has_parent = true;
		nerdT.parent = parent;
		nerdT.is_matrix_dirty = true;
	}

	void update(ecs::state& r_state) override
	{
		bool add_energy = false;
		if (timer <= 0)
		{
			timer = ROUND_TIME;
			add_energy = true;
		}
		else
		{
			timer -= m_timer.smoothed_delta_secs();
		}
		r_state.each<components::player>([&](components::player& player)
		{
			float forward = 0;
			float left = 0;

			r_state.each_id<transforms::transform, components::board>([&](entity_id board_id, auto& board_t, auto& board) {

				if (add_energy && player.energy < 10)
					player.energy++;

				auto& controls = player.team == 1.f ? p1_controls : p2_controls;

				if (player.team == 1.0f) {
					forward = m_input.forward();
					left = m_input.strafe();
				}
				else if (player.team == -1.0f) {
					forward = m_input.forward_player2();
					left = m_input.strafe_player2();
				}


				if (player.state == components::PLAYER_STATE::BASE)
				{
					int loc = find_selected_card_index(controls);

					if (loc != -1) {
						player.selected_card = player.hand[loc];
						player.selected_card_location = loc;
						player.state = components::PLAYER_STATE::UNIT_PLACEMENT;						
					}
					
					if (m_input.is_input_started(controls.dice_button) && player.bonus_dice > 0){
						player.state = components::PLAYER_STATE::DICE_PLACEMENT;
					}
				}
				else if (player.state == components::PLAYER_STATE::UNIT_PLACEMENT)
				{

				    int loc = find_selected_card_index(controls);

				    if (loc != player.selected_card_location && loc != -1)
                    {
                        player.state = components::PLAYER_STATE::BASE;
                    }

					r_state.each<components::board_square, transforms::transform>([&](
					        components::board_square& square,
					        transforms::transform& transform)
						{
							if (square.y == controls.column_to_up_scale)
							{
								square.x == player.selected_row
								    ? row_select(transform, 1.5f)
								    : row_select(transform, 1);
							}
						});

					if (m_input.is_input_started(controls.card1))
					{
						bool taken = false;
						r_state.each<components::game_piece>([&](components::game_piece& piece)
							{
								if (piece.board_source == glm::ivec2(player.selected_row, controls.column_to_up_scale))
								{
									taken = true;
								}
							});

						if (!taken && player.energy >= components::card_costanamos[(int)player.hand[player.selected_card_location]])
						{
							spawn_unit(player.selected_row, player.team, r_state, player.hand[player.selected_card_location]);
							player.energy -= components::card_costanamos[(int)player.hand[player.selected_card_location]];
							player.hand[player.selected_card_location] = player.safe_draw();


							r_state.each<components::board_square, transforms::transform>([&](
							        components::board_square& square,
							        transforms::transform& transform)
								{
									transform.scale.y = 1;
									transform.is_matrix_dirty = true;
								});

							player.state = components::PLAYER_STATE::BASE;
						}
					}
					else if (forward > .4f)
					{
						if (player.selected_row > 0 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_row--;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}
					else if (forward < -.4f)
					{
						if (player.selected_row < 6 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_row++;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}

				}
				else if (player.state == components::PLAYER_STATE::DICE_PLACEMENT)
				{
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{

						bool foo = player.net_check(glm::ivec2(square.x, square.y), glm::ivec2(player.selected_row, player.selected_column),
							components::dice_nets::SEVEN, player.rotate_state, player.flip_state);
						foo ? row_select(transform, 1.5f) : row_select(transform, 1);
						if (m_input.is_input_started(controls.card1) && foo && player.energy >= components::dice_costanamos)
						{
							transform.scale.y = 1;
							square.terrain_type = terrain::fire;
							create_fire_graphic(transform.position, board_id);
							
						}
						transform.is_matrix_dirty = true;
					});

					if (m_input.is_input_started(controls.card1) && player.energy >= components::dice_costanamos) {
						player.bonus_dice--;
						player.energy -= components::dice_costanamos;
						player.state = components::PLAYER_STATE::BASE;
					} 
					else if (m_input.is_input_started(controls.card2))
					{
						r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
							{
								transform.scale.y = 1;
								transform.is_matrix_dirty = true;
							});
						player.state = components::PLAYER_STATE::BASE;
					}
					else if (m_input.is_input_started(controls.card3)) {
						player.flip_state = !player.flip_state;
					}
					else if (m_input.is_input_started(controls.card4)) {
						auto next = static_cast<components::rotate_states>(static_cast<int>(player.rotate_state) + 1);
						if (next == components::rotate_states::NUM) {
							next = components::rotate_states::ZERO;
						}
						player.rotate_state = next;
					}
					else if (forward > .4f)
					{
						if (player.selected_row > 0 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_row--;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}
					else if (forward < -.4f)
					{
						if (player.selected_row < 6 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_row++;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}

					else if (left < -.4f)
					{
						if (player.selected_column > 0 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_column--;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}
					else if (left > .4f)
					{
						if (player.selected_column < 8 && player.select_delay <= 0.f)
						{
							player.select_delay = 0.1f;
							player.selected_column++;
						}
						else
						{
							player.select_delay -= m_timer.smoothed_delta_secs();
						}
					}


				}
			});
			
		});
	}

private:
	float timer = ROUND_TIME;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;

	int find_selected_card_index(player_controls& controls)
    {
        if (m_input.is_input_started(controls.card1))
            return 0;
        else if (m_input.is_input_started(controls.card2))
            return 1;
        else if (m_input.is_input_started(controls.card3))
            return 2;
        else if (m_input.is_input_started(controls.card4))
            return 3;

        return -1;
    }

	player_controls p1_controls {
            core::CARD1_CONTROL,
            core::CARD2_CONTROL,
            core::CARD3_CONTROL,
            core::CARD4_CONTROL,
            core::DIE1_CONTROL,
            0,
	};

    player_controls p2_controls {
            core::CARD1_CONTROL_PLAYER2,
            core::CARD2_CONTROL_PLAYER2,
            core::CARD3_CONTROL_PLAYER2,
            core::CARD4_CONTROL_PLAYER2,
            core::DIE1_CONTROL_PLAYER2,
            8,
    };
};

#endif
