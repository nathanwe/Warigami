#include "dice_loader.hpp"

void components::dice_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::dice::component_bitshift);

	auto& gp = entity.get_component<dice>();

	// Do loading here

}

component_bitset components::dice_loader::components_to_load()
{
	return components::dice::archetype_bit;
}