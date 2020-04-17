#ifndef GAME_READY_DISPLAY_LOADER
#define GAME_READY_DISPLAY_LOADER

#include <asset/component_loader.hpp>
#include "selection_arrow.hpp"


namespace components
{
	class ready_display_loader : public asset::component_loader
	{
	public:
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif