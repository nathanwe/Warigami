#ifndef GAME_COMPONENTS_GAME_PIECE_HPP
#define GAME_COMPONENTS_GAME_PIECE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "components/include/component_bits.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct game_piece : ecs::component<game_piece>
	{
		float does_give_succ = 0.f;
	};
}

#endif
