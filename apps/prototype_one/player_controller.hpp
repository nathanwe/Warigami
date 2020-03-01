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

	virtual void update(ecs::state& r_state) override
	{
		r_state.each<components::player>([&](components::player& player)
		{
			if (state == game::PLAYER_STATE::BASE)
			{
				if (m_input.is_input_started(core::ACTION1_CONTROL))
				{
					selected_card = player.hand[0];
					state = game::PLAYER_STATE::PLACEMENT;
					std::cerr << "CARD 0 SELECTED" << std::endl;
				}
				else if (m_input.is_input_started(core::ACTION2_CONTROL))
				{
					selected_card = player.hand[1];
					state = game::PLAYER_STATE::PLACEMENT;
					std::cerr << "CARD 1 SELECTED" << std::endl;
				}
				else if (m_input.is_input_started(core::ACTION3_CONTROL))
				{
					selected_card = player.hand[2];
					state = game::PLAYER_STATE::PLACEMENT;
					std::cerr << "CARD 2 SELECTED" << std::endl;
				}
				else if (m_input.is_input_started(core::ACTION4_CONTROL))
				{
					selected_card = player.hand[3];
					state = game::PLAYER_STATE::PLACEMENT;
					std::cerr << "CARD 3 SELECTED" << std::endl;
				}
			}
			else if (state == game::PLAYER_STATE::PLACEMENT)
			{

				r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform) 
				{
					if (square.y == 0)
					{
						if (square.x == player.selected_row)
						{
							transform.scale.y = 2;
						}
						else
						{
							transform.scale.y = 1;	
						}
						transform.is_matrix_dirty = true;
					}

				});

				if (m_input.is_input_started(core::ACTION1_CONTROL))
				{
					ecs::entity nerd = hydrater.add_from_prototype("assets/prototypes/basic_unit.json");
					transforms::transform& nerdT = nerd.get_component<transforms::transform>();
					components::game_piece& nerdP = nerd.get_component<components::game_piece>();
					nerdP.board_location.x = player.selected_row;
					nerdP.board_location.y = 0.f;
					nerdP.continuous_board_location = nerdP.board_location;
					nerdP.team = 1.f;
					nerdP.move_board = nerdP.move_board * nerdP.team;
					nerdP.move_world = nerdP.move_world * nerdP.team;
					std::vector<glm::vec2> new_attacks;
					for (int i = 0; i < nerdP.attacks.size(); i++)
					{
						new_attacks.push_back(nerdP.attacks[i] * (int) nerdP.team);
					}
					
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{
						transform.scale.y = 1;
						transform.is_matrix_dirty = true;
					});
					state = game::PLAYER_STATE::BASE;
				}
				else if (m_input.is_input_started(core::ACTION2_CONTROL))
				{
					std::cerr << "PLACEMENT CANCELLED" << std::endl;
					state = game::PLAYER_STATE::BASE;
				}
				else if (m_input.forward() > .4f)
				{
					if (player.selected_row > 0 && row_select_delay <= 0.f)
					{
						row_select_delay = .1f;
						player.selected_row--;
						std::cerr << "Selected Row: " << player.selected_row << std::endl;
					}
					else
					{
						row_select_delay -= m_timer.smoothed_delta_secs();
					}
				}
				else if (m_input.forward() < -.4f)
				{
					if (player.selected_row < 6 && row_select_delay <= 0.f)
					{
						row_select_delay = .1f;
						player.selected_row++;
						std::cerr << "Selected Row: " << player.selected_row << std::endl;
					}
					else
					{
						row_select_delay -= m_timer.smoothed_delta_secs();
					}
				}
			}
		});
	}

private:
	components::card_enum selected_card;
	float row_select_delay = .1f;
	game::PLAYER_STATE state = game::PLAYER_STATE::BASE;
	core::game_input_manager& m_input;
	core::frame_timer& m_timer;
	event::EventManager& event_manager;
	asset::scene_hydrater& hydrater;
};

#endif