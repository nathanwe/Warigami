#ifndef WIZARDPEOPLE_RENDERING_MESH_STATIC_ASSET_HPP
#define WIZARDPEOPLE_RENDERING_MESH_STATIC_ASSET_HPP

#include "vertex.hpp"

#include <string>
#include <vector>

namespace rendering
{
	struct mesh_static_asset
	{
		std::vector<vertex> _vertices;
		std::vector<std::uint32_t> _indices;
	};
}

#endif
