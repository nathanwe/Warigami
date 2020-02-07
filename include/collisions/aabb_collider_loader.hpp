#ifndef COLLISIONS_AABB_COLLIDER_LOADER_HPP
#define COLLISIONS_AABB_COLLIDER_LOADER_HPP

#include <asset/component_loader.hpp>
#include "collisions/collider.hpp"


namespace collisions
{
	class aabb_collider_loader : public asset::component_loader
	{
		void load(asset::asset_loader_node& asset_loader_node) override;
		component_bitset components_to_load() override;
	};

}

#endif