#include <collisions/collision_manager.hpp>
using namespace collisions;

#include <math.h>

// Helpful for collisions
float square_distance(glm::vec3 p1, glm::vec3 p2) {
	return powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2) + powf(p2.z - p1.z, 2);
}

bool check_collision_sphere_sphere(collider* collider1, glm::vec3 pos1, collider* collider2, glm::vec3 pos2) {
	sphere_collider* sphere1 = static_cast<sphere_collider*>(collider1);
	sphere_collider* sphere2 = static_cast<sphere_collider*>(collider2);
	auto a = square_distance(pos1, pos2);
	if (square_distance(pos1, pos2) < powf(sphere1->radius + sphere2->radius, 2)) {
		return true;
	}
	return false;
}

bool check_collision_AABB_AABB(collider* collider1, glm::vec3 pos1, collider* collider2, glm::vec3 pos2) {
	AABB_collider* AABB1 = static_cast<AABB_collider*>(collider1);
	AABB_collider* AABB2 = static_cast<AABB_collider*>(collider2);

	if (pos1.x + AABB1->right >= pos2.x + AABB2->left &&
		pos2.x + AABB2->right >= pos1.x + AABB1->left &&
		pos1.y + AABB1->top >= pos2.y + AABB2->bottom &&
		pos2.y + AABB2->top >= pos1.y + AABB1->bottom &&
		pos1.z + AABB1->back >= pos2.z + AABB2->front &&
		pos2.z + AABB2->back >= pos1.z + AABB1->front) {
		return true;
	}
	return false;
}

bool check_collision_sphere_AABB(collider* collider_sphere, glm::vec3 pos_sphere, collider* collider_AABB, glm::vec3 pos_AABB) {
	AABB_collider* AABB = static_cast<AABB_collider*>(collider_AABB);
	sphere_collider* sphere = static_cast<sphere_collider*>(collider_sphere);

	if (pos_sphere.x + sphere->radius >= pos_AABB.x + AABB->left &&
		pos_sphere.x - sphere->radius <= pos_AABB.x - AABB->right &&
		pos_sphere.y + sphere->radius <= pos_AABB.y + AABB->top &&
		pos_sphere.y - sphere->radius >= pos_AABB.y - AABB->bottom &&
		pos_sphere.z + sphere->radius <= pos_AABB.z + AABB->back &&
		pos_sphere.z - sphere->radius >= pos_AABB.z - AABB->front) {
		return true;
	}

	return false;
}


bool check_collision_AABB_sphere(collider* collider_AABB, glm::vec3 pos_AABB, collider* collider_sphere, glm::vec3 pos_sphere) {
	return check_collision_sphere_AABB(collider_sphere, pos_sphere, collider_AABB, pos_AABB);
}

collision_manager::collision_manager() {
	collision_functions[(unsigned int)collider::collider_type::SPHERE][(unsigned int)collider::collider_type::SPHERE] = check_collision_sphere_sphere;
	collision_functions[(unsigned int)collider::collider_type::SPHERE][(unsigned int)collider::collider_type::AABB] = check_collision_sphere_AABB;
	collision_functions[(unsigned int)collider::collider_type::AABB][(unsigned int)collider::collider_type::SPHERE] = check_collision_AABB_sphere;
	collision_functions[(unsigned int)collider::collider_type::AABB][(unsigned int)collider::collider_type::AABB] = check_collision_AABB_AABB;
}

bool collision_manager::check_collision(collider* collider1, glm::vec3 pos1, collider* collider2, glm::vec3 pos2) {
	return collision_functions[(unsigned int)collider1->type][(unsigned int)collider2->type](collider1, pos1, collider2, pos2);
}