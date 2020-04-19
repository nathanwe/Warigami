#ifndef GAME_COMPONENTS_COMPONENT_BITS_HPP
#define GAME_COMPONENTS_COMPONENT_BITS_HPP

#include "ecs/ecs_types.hpp"

namespace components
{
	enum class component_bits : component_shift
	{
		game_piece = 32,
		board = 33,
		board_square = 34,
		card = 35,
		dice = 36,
		player = 37,
		board_path_movement = 38,
		hit_points = 39,
		disco_light = 40,
		health_meter = 41,
		energy_meter = 42,
		tug_of_war_meter = 43,
        deck_ui = 44,
		terrain = 45,
		countdown = 46,
		deck_selection = 47,
		deck_option = 48,
		selection_arrow = 49,
		pause = 50,
		ready_display = 51,
		main_menu = 52,
		energy_ball = 53,
		logo = 54,
		pause_arrow = 55,
		deck_cursor = 56,
		spawn_effect = 57,
		capture = 58,
		tip = 59,
		button_tooltip = 60

		// careful: need to take action after 
		// count reaches 63.
		// change the component_bitset typedef to be a std::bitset<128>
		// and fix what breaks.
	};
}

#endif
