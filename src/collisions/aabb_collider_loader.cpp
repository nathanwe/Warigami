#include "collisions/aabb_collider_loader.hpp"
#include <iostream>

void collisions::aabb_collider_loader::load(asset::asset_loader_node& asset_loader_node)
{
	std::cerr << "We are in aabb loader" << std::endl;
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data.component_data(collisions::AABB_collider::component_bitshift);
	auto& c = entity.get_component<AABB_collider>();

	c.left = json.value("left", 0.f);
	std::cout << c.left << std::endl;
	c.right = json.value("right", 0.f);
	std::cout << c.right << std::endl;
	c.top = json.value("top", 0.f);
	c.bottom = json.value("bottom", 0.f);
	c.back = json.value("back", 0.f);
	c.front = json.value("front", 0.f);

	if (json.find("relative") != json.end())
	{
		c.position_relative = glm::vec3(
			json["relative"][0].get<float>(),
			json["relative"][1].get<float>(),
			json["relative"][2].get<float>());
		
		if (json.find("transform") != json.end())
		{
			c.position_absolute = glm::vec3(
				json["relative"][0].get<float>() + json["transform"][0],
				json["relative"][1].get<float>() + json["transform"][1],
				json["relative"][2].get<float>() + json["transform"][2]);
		}
	}
}

component_bitset collisions::aabb_collider_loader::components_to_load()
{
	return collisions::AABB_collider::archetype_bit;
}