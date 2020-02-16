#include "asset/asset_manager.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fmod_errors.h"
#include "stb/stb_image.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace asset
{
	void validate_path(const std::string& path)
	{
		if (!std::filesystem::exists(path))
		{
			std::cerr << "Trying to open invalid file " << path << std::endl;
			assert(false);
		}
	}

	template <>
	proto_texture_hdr& asset_manager::get<proto_texture_hdr>(std::string const& filepath)
	{
		validate_path(filepath);
		proto_texture_hdr& proto_tex = _texture_hdr_cache[filepath];
		proto_tex.bytes = stbi_loadf(filepath.c_str(), &proto_tex.width, &proto_tex.height, &proto_tex.channels, 0);
		if (!proto_tex.bytes)
		{
			std::cout << "Error: stbi failed to load texture from file." << std::endl;
			stbi_image_free(proto_tex.bytes);
			assert(false);
		}
		if (proto_tex.channels != 3)
		{
			std::cout << "Error: Unexpected amount of channels while loading texture." << std::endl;
			stbi_image_free(proto_tex.bytes);
			assert(false);
		}
		return _texture_hdr_cache[filepath];
	}

	template <>
	proto_texture& asset_manager::get<proto_texture>(std::string const& filepath)
	{
		validate_path(filepath);
		proto_texture proto_tex;
		proto_tex.bytes = stbi_load(filepath.c_str(), &proto_tex.width, &proto_tex.height, &proto_tex.channels, 0);
		if (!proto_tex.bytes)
		{
			std::cout << "Error: stbi failed to load texture from file." << std::endl;
			assert(false);
		}
		return _texture_cache[filepath];
	}

	template <>
	proto_mesh& asset_manager::get<proto_mesh>(std::string const& filepath)
	{
		validate_path(filepath);
		proto_mesh& proto = _mesh_cache[filepath];

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath,
			aiProcess_ValidateDataStructure |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph |
			aiProcess_CalcTangentSpace);
		if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			std::cout << "Error: Assimp failed to load mesh from file. " << importer.GetErrorString() << std::endl;
			assert(false);
		}

		// Only take the first mesh from the file
		proto.assimp_mesh = scene->mMeshes[0];
		return _mesh_cache[filepath];
	}

	template <>
	proto_shader& asset_manager::get<proto_shader>(std::string const& filepath)
	{
		validate_path(filepath);
		return _shader_cache[filepath];
	}

	template <>
	nlohmann::json& asset_manager::get<nlohmann::json>(std::string const& path)
	{
		validate_path(path);
		auto it = _json_cache.find(path);
		if (it == _json_cache.end())
		{
			std::ifstream i(path);
			nlohmann::json j;
			i >> j;

			_json_cache.insert(std::make_pair(path, j));
		}
		return _json_cache.find(path)->second;
	}

	template <>
	FMOD::Sound*& asset_manager::get<FMOD::Sound*>(std::string const& filepath, FMOD::System* system, FMOD_MODE mode)
	{
		FMOD::Sound*& sound = _sound_cache[filepath];
		system->createSound(filepath.c_str(), mode, nullptr, &sound);
		return sound;
	}

	template<>
	void asset_manager::unload<proto_mesh>(std::string const& filepath)
	{
		auto it = _mesh_cache.find(filepath);
		assert(it != _mesh_cache.end());
		_mesh_cache.erase(it);
	}

	template<>
	void asset_manager::unload<proto_texture>(std::string const& filepath)
	{
		auto it = _texture_cache.find(filepath);
		assert(it != _texture_cache.end());
		proto_texture& proto_tex = it->second;
		stbi_image_free(proto_tex.bytes);
		_texture_cache.erase(it);
	}

	template<>
	void asset_manager::unload<proto_texture_hdr>(std::string const& filepath)
	{
		auto it = _texture_hdr_cache.find(filepath);
		assert(it != _texture_hdr_cache.end());
		proto_texture_hdr& proto_tex = it->second;
		stbi_image_free(proto_tex.bytes);
		_texture_hdr_cache.erase(it);
	}
}