#ifndef WIZARD_GAME_PAUSE_COMPONENT_HPP
#define WIZARD_GAME_PAUSE_COMPONENT_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/entity.hpp"
#include "component_bits.hpp"

namespace components
{
	struct pause : ecs::component<pause>
	{
		bool is_game_paused = false;
		bool is_game_started = false;
		bool is_game_countdown_over = false;
		bool is_game_over = false;
		ecs::entity* p_end_screen = nullptr;
	};
}

#endif
