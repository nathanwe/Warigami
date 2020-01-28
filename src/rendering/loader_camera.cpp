#include "rendering/loader_camera.hpp"

#include "rendering/camera.hpp"

#include <string>

namespace rendering
{
	loader_camera::loader_camera(asset_cache& cache) : 
		_cache(cache)
	{}

	void loader_camera::load(asset::asset_loader_node& asset_loader_node)
	{
		auto& entity = asset_loader_node.entity_resource.entity;
		auto& entity_data = asset_loader_node.entity_resource.entity_data;
		auto& json = entity_data.component_data(camera::component_bitshift);
		auto& cam = entity.get_component<camera>();

		if (auto& it = json.find("field_of_view"); it != json.end())
		{
			cam.field_of_view = it->get<float>();
		}
		if (auto& it = json.find("near_clip_distance"); it != json.end())
		{
			cam.near_clip_distance = it->get<float>();
		}
		if (auto& it = json.find("far_clip_distance"); it != json.end())
		{
			cam.far_clip_distance = it->get<float>();
		}
		if (auto& it = json.find("clear_setting"); it != json.end())
		{
			auto& s = it->get<std::string>();
			if (s == "none")
			{
				cam.clear_setting = camera::clear_mode::none;
			}
			if (s == "flat_color")
			{
				cam.clear_setting = camera::clear_mode::flat_color;
			}
			if (s == "sky_box")
			{
				cam.clear_setting = camera::clear_mode::sky_box;
			}
		}
		if (auto& it = json.find("clear_color"); it != json.end())
		{
			cam.clear_color.x = it->operator[](0).get<float>();
			cam.clear_color.y = it->operator[](1).get<float>();
			cam.clear_color.z = it->operator[](2).get<float>();
		}
		if (auto& it = json.find("clear_sky_box"); it != json.end())
		{
			std::array<std::string, 6> filepaths;
			filepaths[0] = it->operator[](0).get<std::string>();
			filepaths[1] = it->operator[](1).get<std::string>();
			filepaths[2] = it->operator[](2).get<std::string>();
			filepaths[3] = it->operator[](3).get<std::string>();
			filepaths[4] = it->operator[](4).get<std::string>();
			filepaths[5] = it->operator[](5).get<std::string>();
			cam.clear_sky_box = _cache.get<cube_map>(filepaths);
		}
	}

	component_bitset loader_camera::components_to_load()
	{
		return camera::archetype_bit;
	}
}
