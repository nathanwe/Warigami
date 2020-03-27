#ifndef WIZARD_GAME_PAUSE_COMPONENT_HPP
#define WIZARD_GAME_PAUSE_COMPONENT_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

namespace components
{
	struct pause : ecs::component<pause>
	{
		bool is_game_paused = false;
	};
}

#endif
