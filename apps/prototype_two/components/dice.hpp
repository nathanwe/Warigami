#ifndef GAME_COMPONENTS_DICE_HPP
#define GAME_COMPONENTS_DICE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

enum class die_faces {
	water,
	earth,
	fire,
	air,
	num
};

namespace components
{
	struct dice : ecs::component<dice>
	{
		die_faces faces[6] = {die_faces::water, die_faces::water, die_faces::water, die_faces::water, die_faces::water, die_faces::water};
	};
}

#endif
