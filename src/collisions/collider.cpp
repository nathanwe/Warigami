#include <collisions/collider.hpp>
#include <math.h>

using namespace collisions;

collider::collider(collider_type type) : type(type) {
	//mpOwnerBody = nullptr;
}

//// Helpful for collisions
//float square_distance(glm::vec3 p1, glm::vec3 p2) {
//	return powf(powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2) + powf(p2.z - p1.z, 2), 0.5);
//}

sphere_collider::sphere_collider() : collider(collider_type::SPHERE) {
	radius = 0.0f;
}
//
//bool sphere_collider::test_point(glm::vec3 p) {
//	return square_distance(p, owner_body) > powf(radius, 2);
//}

AABB_collider::AABB_collider() : collider(collider_type::SPHERE) {
	left = right = top = bottom = back = front = 0.0f;
}

//bool AABB_collider::test_point(glm::vec3 p) {
//	return (p.x >= -left && p.x <= right
//		&& p.y >= -bottom && p.y <= top / 2
//		&& p.z >= -front / 2 && p.z <= back / 2);
//}