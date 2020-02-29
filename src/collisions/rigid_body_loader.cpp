#include "collisions/rigid_body_loader.hpp"

void collisions::rigid_body_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto entity = asset_loader_node.entity_resource.entity;
	auto entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = entity_data->component_data(collisions::rigid_body::component_bitshift);
	auto& rb = entity->get_component<rigid_body>();

	rb.mass = json.value("mass", 0.f);
	rb.grav_muliplier = json.value("gravity", 1.f);
	rb.lul_friction = json.value("friction", 0.993f);
	rb.inv_mass = 1.f / rb.mass;

	if (json.find("acceleration") != json.end())
	{
		rb.acceleration = glm::vec3(
			json["acceleration"][0].get<float>(),
			json["acceleration"][1].get<float>(),
			json["acceleration"][2].get<float>());
	}

	if (json.find("velocity") != json.end())
	{
		rb.acceleration = glm::vec3(
			json["velocity"][0].get<float>(),
			json["velocity"][1].get<float>(),
			json["velocity"][2].get<float>());
	}

	if (json.find("forces") != json.end())
	{
		rb.acceleration = glm::vec3(
			json["forces"][0].get<float>(),
			json["forces"][1].get<float>(),
			json["forces"][2].get<float>());
	}
}

component_bitset collisions::rigid_body_loader::components_to_load()
{
	return collisions::rigid_body::archetype_bit;
}