#pragma once


#include <core/game_input_manager.hpp>
#include "../components/player.hpp"

struct player_controls
{
	core::controls card1;
	core::controls card2;
	core::controls card3;
	core::controls card4;
	core::controls dice_button;
	core::controls dice_button2;
};

struct player_values
{
	float forward = 0;
	float left = 0;
	glm::vec3 team_color;
};

struct player_specific_data
{
	player_controls& controls;
	player_values values;

	player_specific_data(components::player& player, const core::game_input_manager& input) 
		: controls(player.team == 1.f ? p1_controls : p2_controls)
	{
		//set p1&p2 diffrences		
		auto forward = player.team == 1.0f ? input.forward() : input.forward_player2();
		auto left = player.team == 1.0f ? input.strafe() : input.strafe_player2();
		auto team_color = player.team == 1.0f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
		values = { forward, left, team_color };
	}

private:
	player_controls p1_controls{
		core::CARD1_CONTROL,
		core::CARD2_CONTROL,
		core::CARD3_CONTROL,
		core::CARD4_CONTROL,
		core::DIE1_CONTROL,
		core::DIE2_CONTROL
	};

	player_controls p2_controls{
		core::CARD1_CONTROL_PLAYER2,
		core::CARD2_CONTROL_PLAYER2,
		core::CARD3_CONTROL_PLAYER2,
		core::CARD4_CONTROL_PLAYER2,
		core::DIE1_CONTROL_PLAYER2,
		core::DIE2_CONTROL_PLAYER2
	};
};
