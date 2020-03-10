#ifndef GAME_COMPONENTS_BOARD_SQUARE_HPP
#define GAME_COMPONENTS_BOARD_SQUARE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>



namespace components
{
	struct board_square : ecs::component<board_square>
	{
		int x;
		int y;
	};
}

#endif
