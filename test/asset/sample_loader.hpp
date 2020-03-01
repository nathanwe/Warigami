#ifndef __SAMPLE_LOADER_HPP_
#define __SAMPLE_LOADER_HPP_

#include "components.h"
#include <asset/component_loader.hpp>

class sample_loader : public asset::component_loader
{
	void load(asset::asset_loader_node& asset_loader_node) override
	{
		auto entity = asset_loader_node.entity_resource.entity;
		auto entity_data = asset_loader_node.entity_resource.entity_data;

		auto& transfrom = entity->get_component<transform>();
		// auto& renderable = entity.get_component<renderable>(); 
		// ...

		// do something with both transform and renderable while being 
		// able to traverse the asset_loader_node graph if anything. 

		std::cout << "loading something..." << std::endl;
	}

	component_bitset components_to_load() override
	{
		return Anything;
	}
};


#endif // !__SAMPLE_LOADER_HPP_

