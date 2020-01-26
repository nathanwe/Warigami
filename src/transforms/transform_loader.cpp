#include <transforms/transform_loader.hpp>

#include <glm/vec3.hpp>

void transforms::transform_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(transform::archetype_bit);

	auto& t = entity.get_component<transform>();

	t.position = glm::vec3(
		json["position"][0].get<float>(),
		json["position"][1].get<float>(),
		json["position"][2].get<float>());
	t.rotation = glm::vec3(
		json["rotation"][0].get<float>(),
		json["rotation"][1].get<float>(),
		json["rotation"][2].get<float>());
	t.scale = glm::vec3(
		json["scale"][0].get<float>(),
		json["scale"][1].get<float>(),
		json["scale"][2].get<float>());

	for (auto& child : asset_loader_node.children)
	{
		auto& child_transform = child.entity_resource.entity.get_component<transform>();
		child_transform.has_parent = true;
		child_transform.parent = entity.id();
	}
}

component_bitset transforms::transform_loader::components_to_load()
{
	return transform::archetype_bit;
}