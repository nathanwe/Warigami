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
		GLUE,
		POINTY_HAT_SPIKES,
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
		static uint32_t default_texture_id;
		static uint32_t fire_texture_id;
		static uint32_t fire_texture_id_p1;
		static uint32_t fire_texture_id_p2;
		//type, team, diffus normal or ambient, stage
		static uint32_t textures[(int)TERRAIN_ENUM::NUM][3][3][5];

	};
}

#endif
