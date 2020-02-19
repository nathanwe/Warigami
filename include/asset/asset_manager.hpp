#ifndef WIZARD_ASSETS_MANAGER_HPP
#define WIZARD_ASSETS_MANAGER_HPP

#include <fstream>

#include "asset/proto_mesh.hpp"
#include "asset/proto_model.hpp"
#include "asset/proto_shader.hpp"
#include "asset/proto_texture_hdr.hpp"
#include "asset/proto_texture.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "nlohmann/json.hpp"
#include "stb/stb_image.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace asset
{
	class asset_manager
	{
	public:
		nlohmann::json& get_json(std::string const& filepath);
		proto_mesh& get_proto_mesh(std::string const& filepath);
		proto_model& get_proto_model(std::string const& filepath);
		proto_shader& get_proto_shader(std::string const& filepath);
		proto_texture& get_proto_texture(std::string const& filepath);
		proto_texture_hdr& get_proto_texture_hdr(std::string const& filepath);
		FMOD::Sound*& get_sound(std::string const& filepath, FMOD::System* system, FMOD_MODE mode);

		void unload_json(std::string const& filepath);
		void unload_proto_mesh(std::string const& filepath);
		void unload_proto_shader(std::string const& filepath);
		void unload_proto_texture(std::string const& filepath);
		void unload_proto_texture_hdr(std::string const& filepath);

		template <typename TPath>
		void read_otherise_write(const TPath& filepath, nlohmann::json& data);

	private:
		std::map<std::string, proto_mesh> _mesh_cache;
		std::map<std::string, proto_model> _model_cache;
		std::map<std::string, proto_shader> _shader_cache;
		std::map<std::string, proto_texture> _texture_cache;
		std::map<std::string, proto_texture_hdr> _texture_hdr_cache;
		std::map<std::string, nlohmann::json> _json_cache;
		std::map<std::string, FMOD::Sound*> _sound_cache;
	};

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
