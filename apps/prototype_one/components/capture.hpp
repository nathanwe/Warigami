#ifndef WIZARD_GAME_CAPTURE_COMPONENT_HPP
#define WIZARD_GAME_CAPTURE_COMPONENT_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

namespace components
{
	struct capture : ecs::component<capture>
	{
		float team = 0.f;
	};
}

#endif
