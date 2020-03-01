#include "board_path_movement_loader.hpp"
#include <iostream>

void components::board_path_movement_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::board_path_movement::component_bitshift);
	auto& gp = entity->get_component<board_path_movement>();

	if (json.find("speed") != json.end())
	{
		gp.speed = json["speed"].get<float>();
	}
	// Do loading here

}

component_bitset components::board_path_movement_loader::components_to_load()
{
	return components::board_path_movement::archetype_bit;
}