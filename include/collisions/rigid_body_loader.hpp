#ifndef GAME_COMPONENTS_RIGID_BODY_LOADER_HPP
#define GAME_COMPONENTS_RIGID_BODY_LOADER_HPP

#include <asset/component_loader.hpp>
#include "rigid_body.hpp"


namespace components
{
	class rigid_body_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif