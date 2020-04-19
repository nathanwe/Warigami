#ifndef GAME_COMPONENTS_GAME_PIECE_HPP
#define GAME_COMPONENTS_GAME_PIECE_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"
#include "../combat.hpp"
#include <vector>
#include "card_enum.hpp"

#include <glm/glm.hpp>



namespace components
{
	enum class UNIT_STATE
	{
		ATTACK,
		MOVE,
		WAIT,
		DYING,
		DEAD,
		PREVIEW,
		DANCING,
		STUN,
		NUM
	};

	enum class PROJECTILE {
		NONE,
		PENCIL,
		WEB,
		FIRE
	};

	struct game_piece : ecs::component<game_piece>
	{
		int speed; // How many squares a unit moves per second
		int damage; // How much damage a unit does per attack
		int health; // How much damage a unit can take before dying
		int max_health; // Maximum health of the unit
		int team; // Which team the unit is a part of
		int remaining_speed;
		int give_points = 0; //How many points a unit is currently worth for dice earning
		int stun_duration = -1;
		glm::ivec2 board_source; // A unit's board coordinates
		glm::vec2 continuous_board_location; // Interpolated position on board
		glm::vec3 continuous_board_rotation; //rotation
		glm::ivec2 board_destination; // A unit's discrete tile coordinates
		components::UNIT_STATE state = components::UNIT_STATE::MOVE; // A unit's state of action
		std::vector< glm::ivec2 > attacks; // A list of offsets in board-space that the unit can attack from its own space
		glm::ivec2 move_board; // A vector keeping track of which direction is forward for the unit in board-space
		glm::vec3 move_world; // A vector for keeping track of which direction is forward for the unit in world-space
		std::vector<combats::COMBAT_EFFECTS> effects;
		components::card_enum piece_type; //what kind of unit it is
		components::PROJECTILE projectile_type; // Type of projectile it uses

		components::UNIT_STATE last_rendered_state = UNIT_STATE::MOVE;
		unsigned int last_sprite = 0;
		float time_last_sprite = 0.f;
		std::vector<ecs::entity> health_points;
		std::vector<std::pair<ecs::entity, glm::vec3>> projectiles; // Entity-Destination pair
		bool flash_step_flag = false;
	};
}

#endif
