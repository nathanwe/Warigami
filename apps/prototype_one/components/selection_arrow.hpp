#ifndef GAME_COMPONENTS_SELECTION_ARROW_HPP
#define GAME_COMPONENTS_SELECTION_ARROW_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

namespace components
{
	struct selection_arrow : ecs::component<selection_arrow>
	{
		int team;
	};
}

#endif
