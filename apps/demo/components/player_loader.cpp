#include "player_loader.hpp"

void components::player_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::player::component_bitshift);

	auto& p = entity.get_component<player>();

	if (json.find("energy") != json.end())
	{
		p.energy = json["energy"].get<float>();
	}
	if (json.find("health") != json.end())
	{
		p.health = json["health"].get<float>();
	}

}

component_bitset components::player_loader::components_to_load()
{
	return components::player::archetype_bit;
}