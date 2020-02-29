#ifndef GAME_COMPONENTS_PLAYER_HPP
#define GAME_COMPONENTS_PLAYER_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct player : ecs::component<player>
	{
	    using row_t = std::int16_t;

		float energy = 0.f;
		float health = 0.f;
		row_t selected_row = -1;


		// Needs a list of cards in hand and dice in pool
	};
}

#endif
