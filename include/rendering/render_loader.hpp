#ifndef __RENDER_LOADER_HPP_
#define __RENDER_LOADER_HPP_

#include <asset/component_loader.hpp>
#include "rendering/asset_cache.hpp"

namespace rendering
{

	class render_loader : public asset::component_loader
	{
	public:
		render_loader(asset_cache& cache);

		void load(asset::asset_loader_node& asset_loader_node) override;

		component_bitset components_to_load() override;

	private:
		asset_cache& _cache;
	};

}

#endif
