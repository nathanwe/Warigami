#include "ready_display_loader.hpp"
#include "ready_display.hpp"


void components::ready_display_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::ready_display::component_bitshift);
	auto& model = entity->get_component<components::ready_display>();
	
	auto& ready_text = asset_loader_node.children[0];
	auto& back_text = asset_loader_node.children[1];

	model.ready_entity = ready_text.entity_resource.entity->id();
	model.back_entity = back_text.entity_resource.entity->id();
	model.team = json.value("team", 1.f);
	model.button_x = json.value("button_x", 15.5f);

	for (auto& child : asset_loader_node.children)
	{
		model.children[model.child_count++] = child.entity_resource.entity->id();
	}
}

component_bitset components::ready_display_loader::components_to_load()
{
	return components::ready_display::archetype_bit;
}

