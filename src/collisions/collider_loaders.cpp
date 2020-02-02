#include <collisions/collider_loaders.hpp>

void collisions::loader_collider::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;

	auto& json = asset_loader_node.entity_resource.entity_data.component_data(collisions::collider::component_bitshift);

	if (auto it = json.find("collider_type"); it != json.end())
	{
		if (it->get<std::string>() == "AABB") {
			auto& collider = entity.get_component<collisions::AABB_collider>();

			if (auto it = json.find("left"); it != json.end())
			{
				collider.left = it->get<float>();
			}
			if (auto it = json.find("right"); it != json.end())
			{
				collider.right = it->get<float>();
			}
			if (auto it = json.find("top"); it != json.end())
			{
				collider.top = it->get<float>();
			}
			if (auto it = json.find("bottom"); it != json.end())
			{
				collider.bottom = it->get<float>();
			}
			if (auto it = json.find("back"); it != json.end())
			{
				collider.back = it->get<float>();
			}
			if (auto it = json.find("front"); it != json.end())
			{
				collider.front = it->get<float>();
			}
		}else if (it->get<std::string>() == "sphere") {
			auto& collider = entity.get_component<collisions::sphere_collider>();

			if (auto it = json.find("radius"); it != json.end())
			{
				collider.radius = it->get<float>();
			}
		}
	}
}

component_bitset collisions::loader_collider::components_to_load()
{
	return collisions::collider::archetype_bit;
}