#ifndef __RIGGED_MESH_STATIC_HPP_
#define __RIGGED_MESH_STATIC_HPP_

#include <vector>
#include <rendering/rigged_vertex.hpp>

namespace rendering
{
	struct rigged_mesh_asset
	{
		std::vector<rigged_vertex> vertices;
	};
}

#endif