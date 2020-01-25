#include <transforms/transform_loader.hpp>

void transforms::transform_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& transfrom = entity.get_component<transform>();

	for (auto& child : asset_loader_node.children)
	{
		auto& child_transform = child.entity_resource.entity.get_component<transform>();
		child_transform.has_parent = true;
		child_transform.parent = entity.id();
	}
}