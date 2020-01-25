#ifndef __RENDER_LOADER_HPP_
#define __RENDER_LOADER_HPP_

#include <asset/component_loader.hpp>
#include <rendering/renderable_mesh_static.hpp>

namespace rendering
{

	class render_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;

		component_bitset components_to_load() override;
	};

}

#endif
