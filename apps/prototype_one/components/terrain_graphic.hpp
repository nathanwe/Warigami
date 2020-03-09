#ifndef WIZARD_GAME_COMPONENTS_TERRAIN_GRAPHIC_HPP
#define WIZARD_GAME_COMPONENTS_TERRAIN_GRAPHIC_HPP

#include <ecs/component.hpp>

#include <glm/glm.hpp>

namespace components
{
	struct terrain_graphic : ecs::component<terrain_graphic>
	{
		glm::ivec2 location;
	};
}

#endif
