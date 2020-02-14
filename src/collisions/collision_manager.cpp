#include <collisions/collision_manager.hpp>
using namespace collisions;

#include <math.h>

// Helpful for collisions
float square_distance(glm::vec3 p1, glm::vec3 p2) {
	return powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2) + powf(p2.z - p1.z, 2);
}

bool check_collision_sphere_sphere(collider* collider1, collider* collider2) {
	sphere_collider* sphere1 = static_cast<sphere_collider*>(collider1);
	sphere_collider* sphere2 = static_cast<sphere_collider*>(collider2);
	auto a = square_distance(sphere1->position_absolute, sphere2->position_absolute);
	if (a < powf(sphere1->radius + sphere2->radius, 2)) {
		return true;
	}
	return false;
}

bool check_collision_AABB_AABB(collider* collider1, collider* collider2) {
	AABB_collider* AABB1 = static_cast<AABB_collider*>(collider1);
	AABB_collider* AABB2 = static_cast<AABB_collider*>(collider2);

	if (AABB1->position_absolute.x + AABB1->right >= AABB2->position_absolute.x - AABB2->left &&
		AABB2->position_absolute.x + AABB2->right >= AABB1->position_absolute.x - AABB1->left &&
		AABB1->position_absolute.y + AABB1->top >= AABB2->position_absolute.y - AABB2->bottom &&
		AABB2->position_absolute.y + AABB2->top >= AABB1->position_absolute.y - AABB1->bottom &&
		AABB1->position_absolute.z - AABB1->back <= AABB2->position_absolute.z + AABB2->front &&
		AABB2->position_absolute.z - AABB2->back <= AABB1->position_absolute.z + AABB1->front) {
		return true;
	}
	return false;
}

bool check_collision_sphere_AABB(collider* collider_sphere, collider* collider_AABB) {
	AABB_collider* AABB = static_cast<AABB_collider*>(collider_AABB);
	sphere_collider* sphere = static_cast<sphere_collider*>(collider_sphere);

	if (sphere->position_absolute.x + sphere->radius >= AABB->position_absolute.x + AABB->left &&
		sphere->position_absolute.x - sphere->radius <= AABB->position_absolute.x - AABB->right &&
		sphere->position_absolute.y + sphere->radius <= AABB->position_absolute.y + AABB->top &&
		sphere->position_absolute.y - sphere->radius >= AABB->position_absolute.y - AABB->bottom &&
		sphere->position_absolute.z + sphere->radius <= AABB->position_absolute.z + AABB->back &&
		sphere->position_absolute.z - sphere->radius >= AABB->position_absolute.z - AABB->front) {
		return true;
	}

	return false;
}


bool check_collision_AABB_sphere(collider* collider_AABB, collider* collider_sphere) {
	return check_collision_sphere_AABB(collider_sphere, collider_AABB);
}

collision_manager::collision_manager() {
	collision_functions[(unsigned int)collider::collider_type::SPHERE][(unsigned int)collider::collider_type::SPHERE] = check_collision_sphere_sphere;
	collision_functions[(unsigned int)collider::collider_type::SPHERE][(unsigned int)collider::collider_type::AABB] = check_collision_sphere_AABB;
	collision_functions[(unsigned int)collider::collider_type::AABB][(unsigned int)collider::collider_type::SPHERE] = check_collision_AABB_sphere;
	collision_functions[(unsigned int)collider::collider_type::AABB][(unsigned int)collider::collider_type::AABB] = check_collision_AABB_AABB;
}

bool collision_manager::check_collision(collider* collider1, collider* collider2) {
	return collision_functions[(unsigned int)collider1->type][(unsigned int)collider2->type](collider1, collider2);
}