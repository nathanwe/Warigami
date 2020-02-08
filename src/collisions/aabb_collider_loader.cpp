#include "collisions/aabb_collider_loader.hpp"

void collisions::aabb_collider_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data.component_data(collisions::AABB_collider::component_bitshift);
	auto& c = entity.get_component<AABB_collider>();

	c.left = json.value("left", 0.f);
	c.right = json.value("right", 0.f);
	c.top = json.value("top", 0.f);
	c.bottom = json.value("bottom", 0.f);
	c.back = json.value("back", 0.f);
	c.front = json.value("front", 0.f);
}

component_bitset collisions::aabb_collider_loader::components_to_load()
{
	return collisions::AABB_collider::archetype_bit;
}