#include "rendering/loader_light_directional.hpp"

#include "rendering/asset_cache.hpp"
#include "rendering/light_directional.hpp"

#include <string>

namespace rendering
{
	void loader_light_directional::load(asset::asset_loader_node& asset_loader_node)
	{
		auto& entity = asset_loader_node.entity_resource.entity;
		auto& entity_data = asset_loader_node.entity_resource.entity_data;
		auto& json = entity_data.component_data(light_directional::component_bitshift);
		auto& light = entity.get_component<light_directional>();

		if (auto it = json.find("intensity"); it != json.end())
		{
			light.intensity = it->get<float>();
		}
		if (auto it = json.find("color"); it != json.end())
		{
			light.color.x = it->operator[](0).get<float>();
			light.color.y = it->operator[](1).get<float>();
			light.color.z = it->operator[](2).get<float>();
		}
		if (auto it = json.find("is_shadow_caster"); it != json.end())
		{
			light.is_shadow_caster = it->get<bool>();
		}
	}

	component_bitset loader_light_directional::components_to_load()
	{
		return light_directional::archetype_bit;
	}
}
