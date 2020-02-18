#include <transforms/transform_loader.hpp>
#include <iostream>
#include <glm/vec3.hpp>

void transforms::transform_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(transform::component_bitshift);

	auto& t = entity.get_component<transform>();

	if (json.find("position") != json.end())
	{
		t.position = glm::vec3(
			json["position"][0].get<float>(),
			json["position"][1].get<float>(),
			json["position"][2].get<float>());
	}
	if (json.find("rotation") != json.end())
	{
		t.rotation = glm::vec3(
			glm::radians(json["rotation"][0].get<float>()),
			glm::radians(json["rotation"][1].get<float>()),
			glm::radians(json["rotation"][2].get<float>()));
	}
	if (json.find("scale") != json.end())
	{
		t.scale = glm::vec3(
			json["scale"][0].get<float>(),
			json["scale"][1].get<float>(),
			json["scale"][2].get<float>());
	}
	for (auto& child : asset_loader_node.children)
	{
	    auto child_id = child.entity_resource.entity.id();
	    auto parent_id = entity.id();

		auto& child_transform = child.entity_resource.entity.get_component<transform>();
		child_transform.has_parent = true;
		child_transform.parent = entity.id();
	}
}

component_bitset transforms::transform_loader::components_to_load()
{
	return transform::archetype_bit;
}