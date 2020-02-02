#ifndef WIZARDPEOPLE_COLLISIONS_LOADER_COLLIDERS_HPP
#define WIZARDPEOPLE_COLLISIONS_LOADER_COLLIDERS_HPP

#include "asset/component_loader.hpp"
#include "collisions/collider.hpp"

namespace collisions
{
	class loader_collider : public asset::component_loader
	{
	public:

		void load(asset::asset_loader_node& asset_loader_node) override;

		component_bitset components_to_load() override;

	};
}

#endif
