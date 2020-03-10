#ifndef GAME_COMPONENTS_BOARD_SQUARE_HPP
#define GAME_COMPONENTS_BOARD_SQUARE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>



namespace components
{
	enum class TERRAIN_ENUM {
		fire,
		web,
		glue,
		pointy_hat_spikes,
		num
	};

	struct terrain {
		terrain(TERRAIN_ENUM _type, float _team = 0, int _damage = 1, int _charges = -1, int _duration = -1)
			:type(_type), team(_team), damage(_damage), charges(_charges), duration(_duration) {}
		components::TERRAIN_ENUM type;
		float team;
		int damage;
		int charges;
		int duration;

	};
	struct board_square : ecs::component<board_square>
	{
		int x;
		int y;
		std::vector<terrain> terrains;
	};
}

#endif
