#ifndef WIZARD_GAME_COMPONENTS_TUG_OF_WAR_METER_HPP
#define WIZARD_GAME_COMPONENTS_TUG_OF_WAR_METER_HPP

#include <ecs/component.hpp>

#include <glm/glm.hpp>

namespace components
{
	struct tug_of_war_meter : ecs::component<tug_of_war_meter>
	{
		float value = 0.f; // [-100.f, 100.f]
	};
}

#endif
