#include "rendering/loader_light_point.hpp"

#include "rendering/light_point.hpp"

#include <string>

namespace rendering
{
	void loader_light_point::load(asset::asset_loader_node& asset_loader_node)
	{
		auto& entity = asset_loader_node.entity_resource.entity;
		auto& entity_data = asset_loader_node.entity_resource.entity_data;
		auto& json = entity_data.component_data(light_point::component_bitshift);
		auto& light = entity.get_component<light_point>();

		if (auto& it = json.find("intensity"); it != json.end())
		{
			light.intensity = it->get<float>();
		}
		if (auto& it = json.find("color"); it != json.end())
		{
			light.color.x = it->operator[](0).get<float>();
			light.color.y = it->operator[](1).get<float>();
			light.color.z = it->operator[](2).get<float>();
		}
	}

	component_bitset loader_light_point::components_to_load()
	{
		return light_point::archetype_bit;
	}
}
