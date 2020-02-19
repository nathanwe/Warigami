#include "rendering/loader_model.hpp"

#include "rendering/renderable_model_static.hpp"

#include <string>

namespace rendering
{
	loader_model::loader_model(asset_cache& cache) :
		_cache(cache)
	{}

	void loader_model::load(asset::asset_loader_node& asset_loader_node)
	{
		auto& entity = asset_loader_node.entity_resource.entity;
		auto& entity_data = asset_loader_node.entity_resource.entity_data;
		auto& json = entity_data.component_data(renderable_model_static::component_bitshift);
		auto& renderable = entity.get_component<renderable_model_static>();

		if (auto it = json.find("path"); it != json.end())
		{
			renderable.model_ = _cache.get<model>(it->get<std::string>());
		}
	}

	component_bitset loader_model::components_to_load()
	{
		return renderable_model_static::archetype_bit;
	}
}
