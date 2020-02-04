#include "board_square_loader.hpp"

void components::board_square_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::board_square::component_bitshift);

	auto& gp = entity.get_component<board_square>();

	// Do loading here

}

component_bitset components::board_square_loader::components_to_load()
{
	return components::board_square::archetype_bit;
}