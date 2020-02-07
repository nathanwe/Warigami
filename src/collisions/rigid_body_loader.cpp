#include "rigid_body_loader.hpp"

void components::board_square_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::rigid_body::component_bitshift);

	auto& rb = entity.get_component<rigid_body>();

	// Do loading here

}

component_bitset components::rigid_body_loader::components_to_load()
{
	return components::rigid_body::archetype_bit;
}