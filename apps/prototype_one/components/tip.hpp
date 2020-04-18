#ifndef WIZARD_GAME_TIP_COMPONENT_HPP
#define WIZARD_GAME_TIP_COMPONENT_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/entity.hpp"
#include "component_bits.hpp"

namespace components
{
	struct tip : ecs::component<tip>
	{
		float team;
	};
}

#endif
