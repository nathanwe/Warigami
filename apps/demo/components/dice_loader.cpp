#include "dice_loader.hpp"

void components::dice_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::dice::component_bitshift);

	auto& d = entity.get_component<dice>();

	if (json.find("faces") != json.end())
	{
		d.faces[0] = json["faces"][0].get<die_faces>();
		d.faces[1] = json["faces"][1].get<die_faces>();
		d.faces[2] = json["faces"][2].get<die_faces>();
		d.faces[3] = json["faces"][3].get<die_faces>();
		d.faces[4] = json["faces"][4].get<die_faces>();
		d.faces[5] = json["faces"][5].get<die_faces>();
	}

}

component_bitset components::dice_loader::components_to_load()
{
	return components::dice::archetype_bit;
}