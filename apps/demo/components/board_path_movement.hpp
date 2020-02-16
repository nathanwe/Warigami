#ifndef GAME_COMPONENTS_BOARD_PATH_MOVEMENT_HPP
#define GAME_COMPONENTS_BOARD_PATH_MOVEMENT_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>
#include <list>



namespace components
{
	struct board_path_movement : ecs::component<board_path_movement>
	{
		// Tracking board-state
		//glm::vec2 foo = { 1,1 };
		std::list<glm::vec3> path = { {3,3,3},{-3,3,3},{-3,-3,3},{3,-3,3},{3,-3,-3}, {0,0,0} };
		float speed = 1.0f; // squares/sec
		
	};
}

#endif
