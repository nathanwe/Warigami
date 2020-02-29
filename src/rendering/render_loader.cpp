#include <rendering/render_loader.hpp>

#include <rendering/renderable_mesh_static.hpp>

#include <string>

rendering::render_loader::render_loader(asset_cache& cache) : 
	_cache(cache)
{}

void rendering::render_loader::load(asset::asset_loader_node& asset_loader_node)
{
	auto& entity = asset_loader_node.entity_resource.entity;
	auto& entity_data = asset_loader_node.entity_resource.entity_data;
	auto& json = entity_data->component_data(renderable_mesh_static::component_bitshift);
	auto& renderable = entity->get_component<renderable_mesh_static>();

	if (auto it = json.find("mesh"); it != json.end())
	{
		renderable.mesh = _cache.get<mesh_static>(it->get<std::string>());
	}
	if (auto it = json.find("texture_diffuse"); it != json.end())
	{
		renderable.material.texture_diffuse = _cache.get<texture>(it->get<std::string>()).id;
	}
	if (auto it = json.find("texture_metalness"); it != json.end())
	{
		renderable.material.texture_metalness = _cache.get<texture>(it->get<std::string>()).id;
	}
	if (auto it = json.find("texture_roughness"); it != json.end())
	{
		renderable.material.texture_roughness = _cache.get<texture>(it->get<std::string>()).id;
	}
	if (auto it = json.find("texture_normal"); it != json.end())
	{
		renderable.material.texture_normal = _cache.get<texture>(it->get<std::string>()).id;
	}
	if (auto it = json.find("texture_ambient_occlusion"); it != json.end())
	{
		renderable.material.texture_ambient_occlusion = _cache.get<texture>(it->get<std::string>()).id;
	}
	if (auto it = json.find("texture_scale"); it != json.end())
	{
		renderable.material.texture_scale.x = it->operator[](0).get<float>();
		renderable.material.texture_scale.y = it->operator[](1).get<float>();
	}
	if (auto it = json.find("texture_offset"); it != json.end())
	{
		renderable.material.texture_offset.x = it->operator[](0).get<float>();
		renderable.material.texture_offset.y = it->operator[](1).get<float>();
	}
	if (auto it = json.find("param_diffuse"); it != json.end())
	{
		renderable.material.param_diffuse.x = it->operator[](0).get<float>();
		renderable.material.param_diffuse.y = it->operator[](1).get<float>();
		renderable.material.param_diffuse.z = it->operator[](2).get<float>();
	}
	if (auto it = json.find("param_metalness"); it != json.end())
	{
		renderable.material.param_metalness = it->get<float>();
	}
	if (auto it = json.find("param_roughness"); it != json.end())
	{
		renderable.material.param_roughness = it->get<float>();
	}

}

component_bitset rendering::render_loader::components_to_load()
{
	return renderable_mesh_static::archetype_bit;
}