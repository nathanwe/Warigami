#include "collisions/aabb_collider_loader.hpp"
#include <iostream>

void collisions::aabb_collider_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data.component_data(collisions::AABB_collider::component_bitshift);
	auto& json2 = entity_data.component_data(transforms::transform::component_bitshift);
	auto& c = entity.get_component<AABB_collider>();

	c.owner_id = entity.id();

	c.local_left = json.value("left", 0.5f);
	c.local_right = json.value("right", 0.5f);
	c.local_top = json.value("top", 0.5f);
	c.local_bottom = json.value("bottom", 0.5f);
	c.local_back = json.value("back", 0.5f);
	c.local_front = json.value("front", 0.5f);

	if (json.find("relative") != json.end())
	{
		c.position_relative = glm::vec3(
			json["relative"][0].get<float>(),
			json["relative"][1].get<float>(),
			json["relative"][2].get<float>());
		
		if (json.find("transform") != json.end())
		{
			c.position_absolute = glm::vec3(
				json["relative"][0].get<float>() + json["transform"][0].get<float>(),
				json["relative"][1].get<float>() + json["transform"][1].get<float>(),
				json["relative"][2].get<float>() + json["transform"][2].get<float>());
		}
	}
}

component_bitset collisions::aabb_collider_loader::components_to_load()
{
	return collisions::AABB_collider::archetype_bit | transforms::transform::archetype_bit;
}