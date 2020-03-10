#include "card_loader.hpp"

void components::card_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(components::card::component_bitshift);
	auto& c = entity->get_component<card>();

	c.card_type = json.value("card_type", card_enum::NO_CARD);
	c.speed = json.value("speed", 0.f);
	c.power = json.value("power", 0.f);
	c.armor = json.value("armor", 0.f);
	c.health = json.value("health", 0.f);
	c.cost = json.value("cost", 0.f);
}

component_bitset components::card_loader::components_to_load()
{
	return components::card::archetype_bit;
}