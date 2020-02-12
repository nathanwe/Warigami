#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include "collider.hpp"
#include <list>

namespace collisions {
	class collision_manager {
	public:
		collision_manager();

		bool check_collision(collider* collider1, collider* collider2);

		bool(*collision_functions[(unsigned int)collider::collider_type::NUM_COLLIDER_TYPES][(unsigned int)collider::collider_type::NUM_COLLIDER_TYPES])
			(collider* collider1, collider* collider2);
	};
}

#endif