#include "player_loader.hpp"

void components::player_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::player::component_bitshift);
	auto& p = entity->get_component<player>();
	p.energy = json.value("energy", 5);
	p.health = json.value("health", 100.0f);
	p.team = json.value("team", 1.0f);
	p.spawn_column = json.value("spawn", 0);
	p.score_column = json.value("score", 8);
}

component_bitset components::player_loader::components_to_load()
{
	return components::player::archetype_bit;
}