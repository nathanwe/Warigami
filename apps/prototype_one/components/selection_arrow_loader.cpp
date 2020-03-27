#include "selection_arrow_loader.hpp"

void components::selection_arrow_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::selection_arrow::component_bitshift);
	auto& sa = entity->get_component<selection_arrow>();
	sa.team = json.value("team", 1);
}

component_bitset components::selection_arrow_loader::components_to_load()
{
	return components::selection_arrow::archetype_bit;
}