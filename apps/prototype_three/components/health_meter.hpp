#ifndef WIZARD_GAME_COMPONENTS_HEALTH_METER_HPP
#define WIZARD_GAME_COMPONENTS_HEALTH_METER_HPP

#include <ecs/component.hpp>

#include <glm/glm.hpp>

namespace components
{
	struct health_meter : ecs::component<health_meter>
	{
		float team = 0;
		glm::vec3 first_position;
	};
}

#endif
