#ifndef GAME_COMPONENTS_COMPONENT_BITS_HPP
#define GAME_COMPONENTS_COMPONENT_BITS_HPP

#include "ecs/ecs_types.hpp"

namespace components
{
	enum class component_bits : component_shift
	{
		game_piece = 32
	};
}

#endif
