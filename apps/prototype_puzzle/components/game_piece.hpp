#ifndef GAME_COMPONENTS_GAME_PIECE_HPP
#define GAME_COMPONENTS_GAME_PIECE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct game_piece : ecs::component<game_piece>
	{
		float speed = 0.f;
		float power = 0.f;
		float armor = 0.f;
		float health = 0.f;
	};
}

#endif
