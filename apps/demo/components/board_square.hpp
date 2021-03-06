#ifndef GAME_COMPONENTS_BOARD_SQUARE_HPP
#define GAME_COMPONENTS_BOARD_SQUARE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

enum class terrain {
	water,
	earth,
	fire,
	air,
	empty,
	num
};

namespace components
{
	struct board_square : ecs::component<board_square>
	{
		float x = 0.f;
		float y = 0.f;
		terrain terrain_type = terrain::empty;
	};
}

#endif
