#include <components/include/game_piece_loader.hpp>

void components::game_piece_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::component_bitshift);

	auto& gp = entity.get_component<game_piece>();

	if (json.find("succ") != json.end())
	{
		std::cerr << "succ does indeed succ" << std::endl;
	}

}

component_bitset components::game_piece_loader::components_to_load()
{
	return components::archetype_bit;
}