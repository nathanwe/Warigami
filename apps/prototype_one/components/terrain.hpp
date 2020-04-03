#ifndef WIZARD_GAME_COMPONENTS_TERRAIN_HPP
#define WIZARD_GAME_COMPONENTS_TERRAIN_HPP

#include <ecs/component.hpp>

#include <glm/glm.hpp>

namespace components
{
	enum class TERRAIN_ENUM {
		NONE,
		FIRE,
		WEB,
		ENERGY_FLOWER,
		NUM
	};

	struct terrain : ecs::component<terrain>
	{
		glm::ivec2 location = glm::ivec2(0,0);
		components::TERRAIN_ENUM type = TERRAIN_ENUM::NONE;
		float team = 0.0f;
		int damage = 0;
		int charges = -1;
		int duration = -1;
		// 0,1 = dying, 2 = blowing, 3 = grown, 4+ = growing
		int growth_stage = 0;
		//type, team(-1,0,1), diffus normal or ambient,
		uint32_t textures[(int)TERRAIN_ENUM::NUM][3][3][7];

	};
}

#endif
