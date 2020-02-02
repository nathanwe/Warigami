#include <collisions/collider.hpp>
#include <collisions/component_bits.hpp>
#include <math.h>

using namespace collisions;

collider::collider() { throw "This is an abstract component, do not instantiate"; }

collider::collider(collider_type type) : type(type) {}

sphere_collider::sphere_collider() : collider(collider_type::SPHERE) {
	radius = 0.0f;
}

AABB_collider::AABB_collider() : collider(collider_type::AABB) {
	left = right = top = bottom = back = front = 0.0f;
}

template<> const component_shift ecs::component<sphere_collider>::component_bitshift = (component_shift)component_bits::sphere_collider;
template<> const component_shift ecs::component<AABB_collider>::component_bitshift = (component_shift)component_bits::aabb_collider;
