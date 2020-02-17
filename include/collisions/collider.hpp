#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <glm/vec3.hpp>
#include "ecs/component.hpp"
#include "ecs/ecs_types.hpp"
#include "collisions/component_bits.hpp"

namespace collisions {
	class collider {
	public:
		enum collider_type {
			AABB,
			SPHERE,
			NUM_COLLIDER_TYPES
		};

		collider_type type;

		//Do not use, this is an abstract component
		collider();
	protected:
		collider(collider_type type);
	};

	class sphere_collider :  public collider, public ecs::component<sphere_collider> {
	public:
		sphere_collider();
		glm::vec3 position_relative;
		glm::vec3 position_absolute;
		entity_id owner_id;
		float radius;
	};

	class AABB_collider : public collider, public ecs::component<AABB_collider> {
	public:
		AABB_collider();
		glm::vec3 position_relative;
		glm::vec3 position_absolute;
		entity_id owner_id;
		float left, right, top, bottom, back, front; // offsets from center
		float local_left, local_right, local_top, local_bottom, local_back, local_front;
	};
}

#endif