#ifndef GAME_COMPONENTS_BOARD_HPP
#define GAME_COMPONENTS_BOARD_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include "board_square.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct board : ecs::component<board>
	{
		std::vector< std::vector< std::pair< entity_id, entity_id > > > board_state; // 
		int rows;
		int columns;
	};
}

#endif
