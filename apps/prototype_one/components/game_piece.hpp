#ifndef GAME_COMPONENTS_GAME_PIECE_HPP
#define GAME_COMPONENTS_GAME_PIECE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include <vector>

#include <glm/glm.hpp>

namespace components
{
	struct game_piece : ecs::component<game_piece>
	{
		float speed = 0.f; // How many squares a unit moves per second
		float power = 0.f; // How much damage a unit does per attack
		float health = 0.f; // How much damage a unit can take before dying
		float team = 0.f; // Which team the unit is a part of
		bool shouldMove = false; // Whether or not the unit should be moved during the step
		std::vector< glm::vec2 > attacks(glm::vec2(0.f, 1.f)); // A list of offsets in board-space that the unit can attack from its own space
		glm::vec2 move_board(0.f, 0.f); // A vector keeping track of which direction is forward for the unit in board-space
		glm::vec3 move_dir(0.f, 0.f, 1.f); // A vector for keeping track of which direction is forward for the unit in world-space
	};
}

#endif
