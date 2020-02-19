#ifndef __RIGGED_MESH_HPP_
#define __RIGGED_MESH_HPP_

#include <cstdint>
#include <vector>
#include <assimp/scene.h>
#include <asset/rigged_vertex.hpp>
#include <asset/asset_manager.hpp>


namespace asset
{
	class rigged_mesh
	{
	public:
		rigged_mesh(aiMesh *assimp_mesh);

        std::vector<rigged_vertex>& vertices();
        std::vector<std::uint32_t>& indices();

	private:
		std::vector<rigged_vertex> _vertices;
		std::vector<std::uint32_t> _indices;
        std::uint32_t _num_indices;

        void build_vertices(aiMesh* mesh);
        void set_bone_weights(aiMesh* mesh);
	};
}
#endif