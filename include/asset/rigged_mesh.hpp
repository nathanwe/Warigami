#ifndef __RIGGED_MESH_HPP_
#define __RIGGED_MESH_HPP_

#include <cstdint>
#include <vector>
#include <assimp/scene.h>
#include <asset/rigged_vertex.hpp>



namespace asset
{
	class rigged_mesh
	{
	public:
		rigged_mesh(aiScene* scene);

	private:
		std::vector<rigged_vertex> _vertices;
		std::vector<std::uint32_t> _indices;

	};
}
#endif