#ifndef GAME_COMPONENTS_GAME_PIECE_HPP
#define GAME_COMPONENTS_GAME_PIECE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include <vector>

#include <glm/glm.hpp>



namespace components
{
	enum class UNIT_STATE
	{
		ATTACK,
		MOVE,
		WAIT,
		DYING,
		NUM
	};

	struct game_piece : ecs::component<game_piece>
	{
		float speed; // How many squares a unit moves per second
		float damage; // How much damage a unit does per attack
		float health; // How much damage a unit can take before dying
		float team; // Which team the unit is a part of
		float death_timer; // How long a unit takes to die
		float wait_timer; // Arbitrary wait setting
		glm::vec2 board_location; // A unit's board coordinates
		components::UNIT_STATE state; // A unit's state of action
		std::vector< glm::vec2 > attacks; // A list of offsets in board-space that the unit can attack from its own space
		glm::vec2 move_board; // A vector keeping track of which direction is forward for the unit in board-space
		glm::vec3 move_world; // A vector for keeping track of which direction is forward for the unit in world-space
	};
}

#endif
