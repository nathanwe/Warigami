#ifndef __TRANSFORM_LOADER_HPP_
#define __TRANSFORM_LOADER_HPP_

#include <asset/component_loader.hpp>
#include <transforms/transform.hpp>


namespace transforms
{

	class transform_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif