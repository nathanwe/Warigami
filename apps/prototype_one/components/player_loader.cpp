#include "player_loader.hpp"

void components::player_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::player::component_bitshift);
	auto& p = entity->get_component<player>();
	p.energy = json.value("energy", 0.f);
	p.health = json.value("health", 0.0f);
	p.team = json.value("team", 1.0f);
}

component_bitset components::player_loader::components_to_load()
{
	return components::player::archetype_bit;
}