#include "game_piece_loader.hpp"
#include <iostream>

void components::game_piece_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto* entity = asset_loader_node.entity_resource.entity;
	auto* entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = entity_data->component_data(components::game_piece::component_bitshift);

	auto& gp = entity->get_component<game_piece>();

	if (json.find("speed") != json.end())
	{
		gp.speed = json["speed"].get<float>();
	}
	if (json.find("power") != json.end())
	{
		gp.power = json["power"].get<float>();
	}
	if (json.find("armor") != json.end())
	{
		gp.armor = json["armor"].get<float>();
	}
	if (json.find("health") != json.end())
	{
		gp.health = json["health"].get<float>();
	}

}

component_bitset components::game_piece_loader::components_to_load()
{
	return components::game_piece::archetype_bit;
}