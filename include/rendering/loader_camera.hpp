#ifndef WIZARDPEOPLE_RENDERING_LOADER_CAMERA_HPP
#define WIZARDPEOPLE_RENDERING_LOADER_CAMERA_HPP

#include "asset/component_loader.hpp"
#include "rendering/asset_cache.hpp"

namespace rendering
{
	class loader_camera : public asset::component_loader
	{
	public:
		loader_camera(asset_cache& cache);

		void load(asset::asset_loader_node& asset_loader_node) override;

		component_bitset components_to_load() override;

	private:
		asset_cache& _cache;
	};
}

#endif
