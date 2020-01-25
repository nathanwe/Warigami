#include <rendering/render_loader.hpp>


void rendering::render_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	// TODO
}

component_bitset rendering::render_loader::components_to_load()
{
	return renderable_mesh_static::archetype_bit;
}