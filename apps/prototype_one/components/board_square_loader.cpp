#include "board_square_loader.hpp"

#include "capture.hpp"

void components::board_square_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::board_square::component_bitshift);
	auto& bs = entity->get_component<board_square>();
	bs.x = json.value("x", 0);	
	bs.y = json.value("y", 0);	


	for (auto& child : asset_loader_node.children)
	{
		auto& child_entity = child.entity_resource.entity;
		bool has_capture = child_entity->get_component_opt<components::capture>();
		if (has_capture)
		{
			bs.captured = child_entity;
		}
	}
}

component_bitset components::board_square_loader::components_to_load()
{
	return components::board_square::archetype_bit;
}