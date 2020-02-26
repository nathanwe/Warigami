#include "asset/asset_manager.hpp"

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

	nlohmann::json& asset_manager::get_json(std::string const& path)
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

	proto_mesh& asset_manager::get_proto_mesh(std::string const& filepath)
	{
		validate_path(filepath);
		auto& proto = _mesh_cache[filepath];

		const aiScene* scene = proto.importer.ReadFile(filepath,
			aiProcess_ValidateDataStructure |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph |
			aiProcess_CalcTangentSpace);
		if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			std::cout << "Error: Assimp failed to load mesh from file. " << proto.importer.GetErrorString() << std::endl;
			assert(false);
		}

		// Only take the first mesh from the file
		proto.assimp_mesh = scene->mMeshes[0];
		proto.assimp_scene = scene;
		return _mesh_cache[filepath];
	}

	proto_model& asset_manager::get_proto_model(std::string const& filepath)
	{
		validate_path(filepath);
		auto& proto = _model_cache[filepath];

		const aiScene* scene = proto.importer.ReadFile(filepath,
			aiProcess_ValidateDataStructure |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_OptimizeMeshes |
			aiProcess_OptimizeGraph |
			aiProcess_CalcTangentSpace);
		if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			std::cout << "Error: Assimp failed to load mesh from file. " << proto.importer.GetErrorString() << std::endl;
			assert(false);
		}
		proto.aiscene = scene;

		return proto;
	}

	proto_shader& asset_manager::get_proto_shader(std::string const& filepath)
	{
		validate_path(filepath);

		std::ifstream filestream;
		std::stringstream stringstream;
#ifndef NDEBUG
		filestream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
#endif
			filestream.open(filepath);
			stringstream << filestream.rdbuf();
			filestream.close();
#ifndef NDEBUG
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "Error: Failed to read shader from source file. Log: " << e.what() << std::endl;
			assert(false);
		}
#endif
		_shader_cache[filepath].source = stringstream.str();
		_shader_cache[filepath].path = filepath;
		return _shader_cache[filepath];
	}

	proto_texture& asset_manager::get_proto_texture(std::string const& filepath)
	{
		validate_path(filepath);
		proto_texture& proto_tex = _texture_cache[filepath];
		proto_tex.bytes = stbi_load(filepath.c_str(), &proto_tex.width, &proto_tex.height, &proto_tex.channels, 0);
		if (!proto_tex.bytes)
		{
			std::cout << "Error: stbi failed to load texture from file." << std::endl;
			assert(false);
		}
		return _texture_cache[filepath];
	}

	proto_texture_hdr& asset_manager::get_proto_texture_hdr(std::string const& filepath)
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

	FMOD::Sound*& asset_manager::get_sound(std::string const& filepath, FMOD::System* system, FMOD_MODE mode)
	{
		FMOD::Sound*& sound = _sound_cache[filepath];
		system->createSound(filepath.c_str(), mode, nullptr, &sound);
		return sound;
	}

	void asset_manager::unload_json(std::string const& filepath)
	{
		auto it = _json_cache.find(filepath);
		assert(it != _json_cache.end());
		_json_cache.erase(it);
	}

	void asset_manager::unload_proto_mesh(std::string const& filepath)
	{
		auto it = _mesh_cache.find(filepath);
		assert(it != _mesh_cache.end());
		_mesh_cache.erase(it);
	}

	void asset_manager::unload_proto_shader(std::string const& filepath)
	{
		auto it = _shader_cache.find(filepath);
		assert(it != _shader_cache.end());
		_shader_cache.erase(it);
	}

	void asset_manager::unload_proto_texture(std::string const& filepath)
	{
		auto it = _texture_cache.find(filepath);
		assert(it != _texture_cache.end());
		proto_texture& proto_tex = it->second;
		stbi_image_free(proto_tex.bytes);
		_texture_cache.erase(it);
	}

	void asset_manager::unload_proto_texture_hdr(std::string const& filepath)
	{
		auto it = _texture_hdr_cache.find(filepath);
		assert(it != _texture_hdr_cache.end());
		proto_texture_hdr& proto_tex = it->second;
		stbi_image_free(proto_tex.bytes);
		_texture_hdr_cache.erase(it);
	}

}