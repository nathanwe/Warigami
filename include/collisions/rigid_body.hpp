#ifndef GAME_COLLISIONS_RIGID_BODY_HPP
#define GAME_COLLISIONS_RIGID_BODY_HPP

#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "component_bits.hpp"

#include <glm/glm.hpp>

namespace collisions
{
	struct rigid_body : ecs::component<rigid_body>
	{
		float mass;
		float inv_mass;
		float grav_muliplier;
		float lul_friction;
		glm::vec3 acceleration;
		glm::vec3 velocity;
		glm::vec3 forces;
	};
}

#endif
