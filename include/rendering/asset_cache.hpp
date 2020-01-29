#ifndef WIZARDPEOPLE_RENDERING_ASSET_CACHE_HPP
#define WIZARDPEOPLE_RENDERING_ASSET_CACHE_HPP

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
		~asset_cache();

	public:
		template <typename TAsset, typename TPath>
		TAsset& get(TPath const& filepath);

	private:
		std::map<std::string, cube_map> _cube_maps;
		std::map<std::string, mesh_static> _mesh_statics;
		std::map<std::string, texture> _textures;
	};

	template <>
	cube_map& asset_cache::get<cube_map>(std::array<std::string, 6> const& filepaths);

	template <>
	texture& asset_cache::get<texture>(std::string const& filepath);

	template <>
	mesh_static& asset_cache::get<mesh_static>(std::string const& filepath);
}

#endif
