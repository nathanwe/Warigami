#include <collisions/collider.hpp>
#include <collisions/component_bits.hpp>
#include <math.h>

using namespace collisions;

collider::collider() { throw "This is an abstract component, do not instantiate"; }

collider::collider(collider_type type) : type(type) {}

sphere_collider::sphere_collider() : collider(collider_type::SPHERE) {
	radius = 0.0f;
}

AABB_collider::AABB_collider() : collider(collider_type::SPHERE) {
	left = right = top = bottom = back = front = 0.0f;
}

template<> const component_shift ecs::component<collider>::component_bitshift = (component_shift)component_bits::collider;
