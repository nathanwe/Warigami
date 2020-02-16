#ifndef WIZARD_ASSETS_MANAGER_HPP
#define WIZARD_ASSETS_MANAGER_HPP

#include "asset/proto_mesh.hpp"
#include "asset/proto_shader.hpp"
#include "asset/proto_texture_hdr.hpp"
#include "asset/proto_texture.hpp"

#include "fmod.hpp"
#include "nlohmann/json.hpp"

#include <array>

namespace asset
{
	class asset_manager
	{
	public:
		template <typename TAsset, typename TPath, typename... TDependencies>
		TAsset& get(TPath const& filepath, TDependencies... dependencies);

		template <typename TAsset, typename TPath, typename... TDependencies>
		void unload(TPath const& filepath, TDependencies... dependencies);

		template <typename TPath>
		void read_otherise_write(const TPath& filepath, nlohmann::json& data);

	private:
		std::map<std::string, proto_mesh> _mesh_cache;
		std::map<std::string, proto_shader> _shader_cache;
		std::map<std::string, proto_texture> _texture_cache;
		std::map<std::string, proto_texture_hdr> _texture_hdr_cache;
		std::map<std::string, nlohmann::json> _json_cache;
		std::map<std::string, FMOD::Sound*> _sound_cache;
	};

	template <>
	proto_texture_hdr& asset_manager::get<proto_texture_hdr>(std::string const& filepath);

	template <>
	proto_texture& asset_manager::get<proto_texture>(std::string const& filepath);

	template <>
	proto_mesh& asset_manager::get<proto_mesh>(std::string const& filepath);

	template <>
	proto_shader& asset_manager::get<proto_shader>(std::string const& filepath);

	template <>
	nlohmann::json& asset_manager::get<nlohmann::json>(std::string const& filepath);

	template <>
	FMOD::Sound*& asset_manager::get<FMOD::Sound*>(std::string const& filepath, FMOD::System* system, FMOD_MODE mode);

	template<typename TPath>
	void asset_manager::read_otherise_write(const TPath& filepath, nlohmann::json& data)
	{
		std::fstream stream(filepath);
		if (stream)
		{
			data.clear();
			stream >> data;
		}
		else
		{
			stream.open(filepath, std::fstream::out);
			stream << data;
		}
		stream.close();
	}

}

#endif
