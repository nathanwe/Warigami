#include "card_loader.hpp"

void components::card_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(components::card::component_bitshift);

	auto& c = entity.get_component<card>();

	if (json.find("speed") != json.end())
	{
		c.speed = json["speed"].get<float>();
	}
	if (json.find("power") != json.end())
	{
		c.power = json["power"].get<float>();
	}
	if (json.find("armor") != json.end())
	{
		c.armor = json["armor"].get<float>();
	}
	if (json.find("health") != json.end())
	{
		c.health = json["health"].get<float>();
	}
	if (json.find("cost") != json.end()) 
	{
		c.cost = json["cost"].get<float>();
	}

}

component_bitset components::card_loader::components_to_load()
{
	return components::card::archetype_bit;
}