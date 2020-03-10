#ifndef WIZARD_GAME_COMPONENTS_TERRAIN_HPP
#define WIZARD_GAME_COMPONENTS_TERRAIN_HPP

#include <ecs/component.hpp>

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

	struct terrain : ecs::component<terrain>
	{
		glm::ivec2 location;
		components::TERRAIN_ENUM type;
		float team;
		int damage;
		int charges;
		int duration;
	};
}

#endif
