#ifndef GAME_COMPONENTS_HIT_POINTS_HPP
#define GAME_COMPONENTS_HIT_POINTS_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>
#include <list>




namespace components
{
	struct hit_points : ecs::component<hit_points>
	{
		float max_hp = 100.0f;
		float current_hp = 100.0f;

		float range = 10.0f;
		float damage = 4.0f;

	};
}

#endif
