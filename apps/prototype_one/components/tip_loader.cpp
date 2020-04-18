#include "tip_loader.hpp"

void components::tip_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::tip::component_bitshift);
	auto& p = entity->get_component<tip>();
	p.team = json.value("team", 1.0f);

}

component_bitset components::tip_loader::components_to_load()
{
	return components::tip::archetype_bit;
}