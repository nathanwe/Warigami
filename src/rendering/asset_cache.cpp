#include "rendering/asset_cache.hpp"

#include "asset/proto_mesh.hpp"

#include "asset/proto_model.hpp"
#include "asset/proto_shader.hpp"

#include "asset/proto_texture.hpp"
#include "asset/proto_texture_hdr.hpp"
#include "rendering/vertex.hpp"

#include "stb/stb_image.h"
#include "glbinding/gl/gl.h"

#include <array>
#include <vector>

#include <iostream>
#include <string>
#include <asset/rigged_mesh.hpp>

using namespace gl;

namespace rendering
{
	asset_cache::~asset_cache()
	{
		using namespace gl;

		std::vector<uint32_t> textures;
		for (const auto& it : _cube_maps)
		{
			if (it.second.id != 0)
			{
				textures.push_back(it.second.id);
			}
		}
		for (const auto& it : _textures)
		{
			if (it.second.id != 0)
			{
				textures.push_back(it.second.id);
			}
		}
		glDeleteTextures(textures.size(), &textures[0]);

		std::vector<uint32_t> vertex_arrays;
		std::vector<uint32_t> buffers;
		for (const auto& it : _mesh_statics)
		{
			if (it.second.vao != 0)
			{
				vertex_arrays.push_back(it.second.vao);
			}
			if (it.second.vbo != 0)
			{
				buffers.push_back(it.second.vbo);
			}
			if (it.second.ebo != 0)
			{
				buffers.push_back(it.second.ebo);
			}
		}
		glDeleteVertexArrays(vertex_arrays.size(), &vertex_arrays[0]);
		glDeleteBuffers(buffers.size(), &buffers[0]);

		/*
		for (auto it : _models)
		{
			for (auto& mesh : it.second.sub_models)
			{

			}
		}
		*/
	}

    template <>
	cube_map& asset_cache::get<cube_map>(std::array<std::string, 6> const& filepaths)
	{
		using namespace gl;

		auto& r_cubemap = _cube_maps[filepaths[0]];
		if (r_cubemap.id == 0)
		{
			glGenTextures(1, &r_cubemap.id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, r_cubemap.id);
			assert(r_cubemap.id != 0);

			for (int i_face = 0; i_face < filepaths.size(); ++i_face)
			{
				asset::proto_texture_hdr& proto_tex = _assets.get_proto_texture_hdr(filepaths[i_face]);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, 0, GL_RGB16F, proto_tex.width, proto_tex.height, 0, GL_RGB, GL_FLOAT, proto_tex.bytes);
				_assets.unload_proto_texture_hdr(filepaths[i_face]);
			}

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
		return r_cubemap;
	}

	template <>
	texture& asset_cache::get<texture>(std::string const& filepath)
	{
		using namespace gl;

		auto& r_texture = _textures[filepath];
		if (r_texture.id == 0)
		{
			asset::proto_texture& proto_tex = _assets.get_proto_texture(filepath);

			GLenum texture_channels_gl;
			switch (proto_tex.channels)
			{
			case 1:
				texture_channels_gl = GL_RED;
				break;
			case 2:
				texture_channels_gl = GL_RG;
				break;
			case 3:
				texture_channels_gl = GL_RGB;
				break;
			case 4:
				texture_channels_gl = GL_RGBA;
				break;

			default:
				std::cout << "Error: Unexpected amount of channels while loading texture." << std::endl;
				_assets.unload_proto_texture(filepath);
				assert(false);
			}

			// Create OpenGL representation
			glGenTextures(1, &r_texture.id);
			glBindTexture(GL_TEXTURE_2D, r_texture.id);

			// Copy data to VRAM
			glTexImage2D(GL_TEXTURE_2D, 0, texture_channels_gl, proto_tex.width, proto_tex.height, 0, texture_channels_gl, GL_UNSIGNED_BYTE, proto_tex.bytes);

			// OpenGL texture settings
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			_assets.unload_proto_texture(filepath);
		}
		return r_texture;
	}

	template <>
	mesh_static& asset_cache::get<mesh_static>(std::string const& filepath)
	{

		auto& r_mesh = _mesh_statics[filepath];
		if (r_mesh.vao == 0)
		{
			asset::proto_mesh& proto = _assets.get_proto_mesh(filepath);
			r_mesh = mesh_from_aimesh(proto.assimp_scene, proto.assimp_mesh);

			//_assets.unload_proto_mesh(filepath);
		}
		return r_mesh;
	}

    template <>
    model& asset_cache::get<model>(std::string const& filepath)
    {
        auto& m = _models[filepath];
        if (m.sub_models.size() == 0)
        {
            auto& proto = _assets.get_proto_model(filepath);
            auto directory = filepath.substr(0, filepath.find_last_of('/')) + '/';
            m = model_from_aiscene(directory, proto.aiscene, proto.aiscene->mRootNode);
        }
        return m;
    }

    model asset_cache::model_from_aiscene(const std::string& directory, const aiScene* aiscene, aiNode* ainode)
    {
        model m;
        for (unsigned int i = 0; i < ainode->mNumMeshes; ++i)
        {
            auto aimesh = aiscene->mMeshes[ainode->mMeshes[i]];
            sub_model sub = sub_model_from_aimesh(directory, aimesh, aiscene);
            m.sub_models.push_back(sub);
        }
        return m;
    }

    sub_model asset_cache::sub_model_from_aimesh(const std::string& directory, aiMesh* aimesh, const aiScene* aiscene)
    {
        sub_model sub;
        sub.mesh = mesh_from_aimesh(aiscene, aimesh);
        sub.material = material_from_aimesh(directory, aimesh, aiscene);
        return sub;
    }

    material_pbr asset_cache::material_from_aimesh(const std::string& directory, aiMesh* aimesh, const aiScene* aiscene)
    {
        aiMaterial* aimat = aiscene->mMaterials[aimesh->mMaterialIndex];
        material_pbr mat;
        texture_from_assimp(mat.texture_diffuse          , directory, aimat, aiTextureType_DIFFUSE, 0);
        texture_from_assimp(mat.texture_metalness        , directory, aimat, aiTextureType_SPECULAR, 0);
        texture_from_assimp(mat.texture_roughness        , directory, aimat, aiTextureType_SHININESS, 0);
        texture_from_assimp(mat.texture_normal           , directory, aimat, aiTextureType_NORMALS, 0);
        texture_from_assimp(mat.texture_ambient_occlusion, directory, aimat, aiTextureType_AMBIENT_OCCLUSION, 0);
        return mat;
    }


	mesh_static asset_cache::mesh_from_aimesh(const aiScene* scene, aiMesh* mesh)
	{
        mesh_static r_mesh;

        if (mesh->mNumBones > 0)
        {
            make_rigged_mesh(r_mesh, scene, mesh);
        } else
        {
            make_static_mesh(r_mesh, mesh);
        }

		return r_mesh;
	}


	void asset_cache::texture_from_assimp(unsigned int& out, const std::string& directory, aiMaterial* aimat, aiTextureType type, unsigned int i)
	{
		if (aimat->GetTextureCount(type) <= 0)
		{
			return;
		}

		aiString filename;
		aimat->GetTexture(type, i, &filename);
		out = get<texture>(directory + std::string(filename.C_Str())).id;
	}

    void asset_cache::make_static_mesh(mesh_static& r_mesh, aiMesh* aimesh)
    {
        using namespace gl;

        // Reorganize data into array of struct instead of separate arrays
        std::vector<vertex> vertices;
        std::vector<uint32_t> indices;
        vertex swap_vertex;
        for (size_t i = 0; i < aimesh->mNumVertices; i++)
        {
            swap_vertex.position.x = aimesh->mVertices[i].x;
            swap_vertex.position.y = aimesh->mVertices[i].y;
            swap_vertex.position.z = aimesh->mVertices[i].z;
            if (aimesh->HasNormals())
            {
                swap_vertex.normal.x = aimesh->mNormals[i].x;
                swap_vertex.normal.y = aimesh->mNormals[i].y;
                swap_vertex.normal.z = aimesh->mNormals[i].z;
            }
            if (aimesh->HasTextureCoords(0))
            {
                swap_vertex.tex_coord.x = aimesh->mTextureCoords[0][i].x;
                swap_vertex.tex_coord.y = aimesh->mTextureCoords[0][i].y;
            }
            if (aimesh->HasTangentsAndBitangents())
            {
                swap_vertex.tangent.x = aimesh->mTangents[i].x;
                swap_vertex.tangent.y = aimesh->mTangents[i].y;
                swap_vertex.tangent.z = aimesh->mTangents[i].z;
                swap_vertex.bitangent.x = aimesh->mBitangents[i].x;
                swap_vertex.bitangent.y = aimesh->mBitangents[i].y;
                swap_vertex.bitangent.z = aimesh->mBitangents[i].z;
            }
            vertices.push_back(swap_vertex);
        }
        for (size_t i = 0; i < aimesh->mNumFaces; i++)
        {
            indices.emplace_back(aimesh->mFaces[i].mIndices[0]);
            indices.emplace_back(aimesh->mFaces[i].mIndices[1]);
            indices.emplace_back(aimesh->mFaces[i].mIndices[2]);
        }
        r_mesh.num_indices = indices.size();

        // Create OpenGL representation
        glGenVertexArrays(1, &r_mesh.vao);
        glGenBuffers(1, &r_mesh.vbo);
        glGenBuffers(1, &r_mesh.ebo);

        // Copy data to VRAM
        glBindVertexArray(r_mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, r_mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        // Register data layout
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tex_coord));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, bitangent));

        glBindVertexArray(0);
    }

    void asset_cache::make_rigged_mesh(mesh_static &r_mesh, const aiScene* scene, aiMesh* mesh)
    {
        using namespace gl;
        asset::rigged_mesh ai_mesh(scene, mesh);
        auto& vertices = ai_mesh.vertices();
        auto& indices = ai_mesh.indices();
        r_mesh.num_indices = ai_mesh.indices().size();


        // Create OpenGL representation
        glGenVertexArrays(1, &r_mesh.vao);
        glGenBuffers(1, &r_mesh.vbo);
        glGenBuffers(1, &r_mesh.ebo);

        // Copy data to VRAM
        glBindVertexArray(r_mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, r_mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(asset::rigged_vertex), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        // Register data layout
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, tex_coord));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, bitangent));
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, bone_ids));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(asset::rigged_vertex), (void*)offsetof(asset::rigged_vertex, weights));

        glBindVertexArray(0);
    }
}
