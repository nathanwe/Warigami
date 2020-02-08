#include "collisions/sphere_collider_loader.hpp"

void collisions::sphere_collider_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data.component_data(collisions::sphere_collider::component_bitshift);
	auto& c = entity.get_component<sphere_collider>();
	c.radius = json.value("radius", 0.f);
}

component_bitset collisions::sphere_collider_loader::components_to_load()
{
	return collisions::sphere_collider::archetype_bit;
}