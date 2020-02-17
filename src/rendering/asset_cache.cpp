#include "rendering/asset_cache.hpp"

#include "asset/proto_mesh.hpp"
#include "asset/proto_shader.hpp"
#include "asset/proto_texture.hpp"
#include "asset/proto_texture_hdr.hpp"
#include "rendering/vertex.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "stb/stb_image.h"
#include "glbinding/gl/gl.h"

#include <array>
#include <vector>


#include <iostream>
#include <fstream>
#include <string>

namespace rendering
{
	asset_cache::~asset_cache()
	{
		using namespace gl;

		std::vector<uint32_t> textures;
		for (auto it : _cube_maps)
		{
			if (it.second.id != 0)
			{
				textures.push_back(it.second.id);
			}
		}
		for (auto it : _textures)
		{
			if (it.second.id != 0)
			{
				textures.push_back(it.second.id);
			}
		}
		glDeleteTextures(textures.size(), &textures[0]);

		std::vector<uint32_t> vertex_arrays;
		std::vector<uint32_t> buffers;
		for (auto it : _mesh_statics)
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			_assets.unload_proto_texture(filepath);
		}
		return r_texture;
	}

	template <>
	mesh_static& asset_cache::get<mesh_static>(std::string const& filepath)
	{
		using namespace gl;

		auto& r_mesh = _mesh_statics[filepath];
		if (r_mesh.vao == 0)
		{
			asset::proto_mesh& proto = _assets.get_proto_mesh(filepath);

			// Reorganize data into array of struct instead of separate arrays
			std::vector<vertex> vertices;
			std::vector<uint32_t> indices;
			vertex swap_vertex;
			for (size_t i = 0; i < proto.assimp_mesh->mNumVertices; i++)
			{
				swap_vertex.position.x = proto.assimp_mesh->mVertices[i].x;
				swap_vertex.position.y = proto.assimp_mesh->mVertices[i].y;
				swap_vertex.position.z = proto.assimp_mesh->mVertices[i].z;
				if (proto.assimp_mesh->HasNormals())
				{
					swap_vertex.normal.x = proto.assimp_mesh->mNormals[i].x;
					swap_vertex.normal.y = proto.assimp_mesh->mNormals[i].y;
					swap_vertex.normal.z = proto.assimp_mesh->mNormals[i].z;
				}
				if (proto.assimp_mesh->HasTextureCoords(0))
				{
					swap_vertex.tex_coord.x = proto.assimp_mesh->mTextureCoords[0][i].x;
					swap_vertex.tex_coord.y = proto.assimp_mesh->mTextureCoords[0][i].y;
				}
				if (proto.assimp_mesh->HasTangentsAndBitangents())
				{
					swap_vertex.tangent.x = proto.assimp_mesh->mTangents[i].x;
					swap_vertex.tangent.y = proto.assimp_mesh->mTangents[i].y;
					swap_vertex.tangent.z = proto.assimp_mesh->mTangents[i].z;
					swap_vertex.bitangent.x = proto.assimp_mesh->mBitangents[i].x;
					swap_vertex.bitangent.y = proto.assimp_mesh->mBitangents[i].y;
					swap_vertex.bitangent.z = proto.assimp_mesh->mBitangents[i].z;
				}
				vertices.push_back(swap_vertex);
			}
			for (size_t i = 0; i < proto.assimp_mesh->mNumFaces; i++)
			{
				indices.emplace_back(proto.assimp_mesh->mFaces[i].mIndices[0]);
				indices.emplace_back(proto.assimp_mesh->mFaces[i].mIndices[1]);
				indices.emplace_back(proto.assimp_mesh->mFaces[i].mIndices[2]);
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

			_assets.unload_proto_mesh(filepath);
		}
		return r_mesh;
	}
}
