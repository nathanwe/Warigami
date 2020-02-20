#ifndef WIZARDPEOPLE_RENDERING_ASSET_CACHE_HPP
#define WIZARDPEOPLE_RENDERING_ASSET_CACHE_HPP

#include "asset/asset_manager.hpp"
#include "rendering/cube_map.hpp"
#include "rendering/mesh_static.hpp"
#include "rendering/texture.hpp"

#include <map>
#include <string>

namespace rendering
{
	// does not yet count references for freeing VRAM
	class asset_cache
	{
	public:
		asset_cache(asset::asset_manager& assets) : _assets(assets) {};
		~asset_cache();

	public:
		template <typename TAsset, typename TPath>
		TAsset& get(TPath const& filepath);

	private:
		asset::asset_manager& _assets;
		std::map<std::string, cube_map> _cube_maps;
		std::map<std::string, mesh_static> _mesh_statics;
		std::map<std::string, texture> _textures;

		static void make_static_mesh(mesh_static& mesh, asset::proto_mesh& proto);
		static void make_rigged_mesh(mesh_static& mesh, asset::proto_mesh& proto);
	};

	template <>
	cube_map& asset_cache::get<cube_map>(std::array<std::string, 6> const& filepaths);

	template <>
	texture& asset_cache::get<texture>(std::string const& filepath);

	template <>
	mesh_static& asset_cache::get<mesh_static>(std::string const& filepath);
}

#endif
