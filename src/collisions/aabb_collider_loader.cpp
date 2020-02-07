#include "collisions/aabb_collider_loader.hpp"

void collisions::aabb_collider_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(collisions::AABB_collider::component_bitshift);

	auto& c = entity.get_component<AABB_collider>();

	if (json.find("left") != json.end())
	{
		c.left = json["left"].get<float>();
	}
	if (json.find("right") != json.end())
	{
		c.right = json["right"].get<float>();
	}
	if (json.find("top") != json.end())
	{
		c.top = json["top"].get<float>();
	}
	if (json.find("bottom") != json.end())
	{
		c.bottom = json["bottom"].get<float>();
	}
	if (json.find("back") != json.end())
	{
		c.back = json["back"].get<float>();
	}
	if (json.find("front") != json.end())
	{
		c.front = json["front"].get<float>();
	}
}

component_bitset collisions::aabb_collider_loader::components_to_load()
{
	return collisions::AABB_collider::archetype_bit;
}