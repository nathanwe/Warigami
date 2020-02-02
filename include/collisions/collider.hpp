#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <glm/vec3.hpp>

namespace collisions {
	class collider {
	public:
		enum collider_type {
			AABB,
			SPHERE,
			NUM_COLLIDER_TYPES
		};
		collider(collider_type type);

		//virtual bool test_point(glm::vec3 p) = 0;

		//Body* mpOwnerBody;
		collider_type type;
	private:

	};

	class sphere_collider : public collider {
	public:
		sphere_collider();
		//bool test_point(glm::vec3 p);

		float radius;
	};

	class AABB_collider : public collider {
	public:
		AABB_collider();
		//bool test_point(glm::vec3 p);

		float left, right, top, bottom, back, front; // offsets from center
	};
}

#endif