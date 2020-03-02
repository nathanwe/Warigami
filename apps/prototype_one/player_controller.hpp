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

	components::card_enum card_select(components::player& player, int num)
	{
		state = game::PLAYER_STATE::PLACEMENT;
		return player.hand[num];
	}

	void row_select(transforms::transform& transform, int num)
	{
		transform.scale.y = num;
		transform.is_matrix_dirty = true;
	}

	void spawn_unit(int lane, int team)
	{
		ecs::entity nerd = hydrater.add_from_prototype("assets/prototypes/basic_unit.json");
		transforms::transform& nerdT = nerd.get_component<transforms::transform>();
		components::game_piece& nerdP = nerd.get_component<components::game_piece>();
		
		nerdP.team = team;
		nerdP.board_location.x = lane;
		nerdP.team >= 0 ? nerdP.board_location.y = 0 : nerdP.board_location.y = 8;
		
		if (nerdP.team >= 0)
			nerdT.rotation.y = AI_MATH_PI;

		nerdP.continuous_board_location = nerdP.board_location;
		nerdP.move_board = nerdP.move_board * nerdP.team;
		nerdP.move_world = nerdP.move_world * nerdP.team;

		std::vector<glm::ivec2> new_attacks;
		for (int i = 0; i < nerdP.attacks.size(); i++)
		{
			new_attacks.push_back(nerdP.attacks[i] * (int)nerdP.team);
		}
	}

	virtual void update(ecs::state& r_state) override
	{
		r_state.each<components::player>([&](components::player& player)
		{
			if (state == game::PLAYER_STATE::BASE)
			{
				if (m_input.is_input_started(core::ACTION1_CONTROL)) 
					selected_card = card_select(player, 0);
				else if (m_input.is_input_started(core::ACTION2_CONTROL))
					selected_card = card_select(player, 1);
				else if (m_input.is_input_started(core::ACTION3_CONTROL))
					selected_card = card_select(player, 2);
				else if (m_input.is_input_started(core::ACTION4_CONTROL))
					selected_card = card_select(player, 3);
			}
			else if (state == game::PLAYER_STATE::PLACEMENT)
			{
				r_state.each<components::board_square, transforms::transform>([&](	components::board_square& square, 
																					transforms::transform& transform) 
				{
					if (1/*player.team >= 0*/)
						square.y == 0 && square.x == player.selected_row ? row_select(transform, 2) : row_select(transform, 1);
					else
						square.y == 8 && square.x == player.selected_row ? row_select(transform, 2) : row_select(transform, 1);
				});

				if (m_input.is_input_started(core::ACTION1_CONTROL))
				{
					spawn_unit(player.selected_row, 1/*player.team*/);
					
					r_state.each<components::board_square, transforms::transform>([&](components::board_square& square, transforms::transform& transform)
					{
						transform.scale.y = 1;
						transform.is_matrix_dirty = true;
					});

					state = game::PLAYER_STATE::BASE;
				}
				else if (m_input.is_input_started(core::ACTION2_CONTROL))
				{
					state = game::PLAYER_STATE::BASE;
				}
				else if (m_input.forward() > .4f)
				{
					if (player.selected_row > 0 && row_select_delay <= 0.f)
					{
						row_select_delay = .05f;
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
						row_select_delay = .05f;
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