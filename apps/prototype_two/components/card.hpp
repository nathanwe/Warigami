#ifndef GAME_COMPONENTS_CARD_HPP
#define GAME_COMPONENTS_CARD_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include "card_enum.hpp"

#include <glm/glm.hpp>

namespace components
{
	struct card : ecs::component<card>
	{
	    card_enum card_type {card_enum::NO_CARD};

		float speed = 0.f;
		float power = 0.f;
		float armor = 0.f;
		float health = 0.f;
		float cost = 0.f;
		float timer = 5.0f;
	};
}

#endif
