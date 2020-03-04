#include "board_loader.hpp"

void components::board_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::board::component_bitshift);
	auto& b = entity->get_component<board>();
	
	std::vector<ecs::entity*> new_entities;

	b.rows = json.value("rows", 7);
	b.columns = json.value("columns", 9);
	b.board_state.resize(b.columns);

	for (int i = 0; i < b.board_state.size(); i++)
	{
		b.board_state[i].resize(b.rows);
	}
}

void components::board_loader::find_ids(asset::asset_loader_node& asset_loader_node, std::vector<ecs::entity*> entities)
{
	if (asset_loader_node.entity_resource.entity->has<components::board_square>())
	{
		entities.push_back(asset_loader_node.entity_resource.entity);
	}
	for (auto& child : asset_loader_node.children)
	{
		find_ids(child, entities);
	}
}

component_bitset components::board_loader::components_to_load()
{
	return components::board::archetype_bit;
}