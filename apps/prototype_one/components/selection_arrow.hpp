#ifndef GAME_COMPONENTS_SELECTION_ARROW_HPP
#define GAME_COMPONENTS_SELECTION_ARROW_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/entity.hpp"
#include "component_bits.hpp"

#include <vector>

namespace components
{
	struct selection_arrow : ecs::component<selection_arrow>
	{
		int team;
		std::vector<ecs::entity> energy_orbs;
	};
}

#endif
