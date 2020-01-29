#ifndef WIZARDPEOPLE_RENDERING_LOADER_LIGHT_POINT_HPP
#define WIZARDPEOPLE_RENDERING_LOADER_LIGHT_POINT_HPP

#include "asset/component_loader.hpp"

namespace rendering
{
	class loader_light_point : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;

		component_bitset components_to_load() override;
	};
}

#endif
