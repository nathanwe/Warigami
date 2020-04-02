#ifndef WIZARD_GAME_COMPONENTS_ENERGY_METER_HPP
#define WIZARD_GAME_COMPONENTS_ENERGY_METER_HPP

#include <ecs/component.hpp>

#include <glm/glm.hpp>

namespace components
{
	struct energy_meter : ecs::component<energy_meter>
	{
		float team = 0;
		glm::vec3 first_position;
		float value;
	};
}

#endif
