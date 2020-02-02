#include "rendering/asset_cache.hpp"

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
#include <filesystem>

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

			int texture_width, texture_height, texture_channels;
			float* texture_bytes;
			for (int i_face = 0; i_face < filepaths.size(); ++i_face)
			{
				texture_bytes = stbi_loadf(filepaths[i_face].c_str(), &texture_width, &texture_height, &texture_channels, 0);
				if (!texture_bytes)
				{
					std::cout << "Error: stbi failed to load texture from file." << std::endl;
					stbi_image_free(texture_bytes);
					assert(false);
				}
				if (texture_channels != 3)
				{
					std::cout << "Error: Unexpected amount of channels while loading texture." << std::endl;
					stbi_image_free(texture_bytes);
					assert(false);
				}

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, 0, GL_RGB16F, texture_width, texture_height, 0, GL_RGB, GL_FLOAT, texture_bytes);
				stbi_image_free(texture_bytes);
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
	json& asset_cache::get<json>(std::string const& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			std::cerr << "Trying to open invalid file " << filepath << std::endl;
			assert(false);
		}

		auto it = _json.find(filepath);

		if (it == _json.end())
		{
			std::ifstream i(filepath);
			json j;
			i >> j;

			_json.insert(std::make_pair(filepath, j));
		}

		return _json.find(filepath)->second;
	}

	template <>
	texture& asset_cache::get<texture>(std::string const& filepath)
	{
		using namespace gl;

		auto& r_texture = _textures[filepath];
		if (r_texture.id == 0)
		{
			int texture_width, texture_height, texture_channels;
			unsigned char* texture_bytes;

			texture_bytes = stbi_load(filepath.c_str(), &texture_width, &texture_height, &texture_channels, 0);
			if (!texture_bytes)
			{
				std::cout << "Error: stbi failed to load texture from file." << std::endl;
				assert(false);
			}

			GLenum texture_channels_gl;
			switch (texture_channels)
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
				stbi_image_free(texture_bytes);
				assert(false);
			}

			// Create OpenGL representation
			glGenTextures(1, &r_texture.id);
			glBindTexture(GL_TEXTURE_2D, r_texture.id);

			// Copy data to VRAM
			glTexImage2D(GL_TEXTURE_2D, 0, texture_channels_gl, texture_width, texture_height, 0, texture_channels_gl, GL_UNSIGNED_BYTE, texture_bytes);

			// OpenGL texture settings
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(texture_bytes);
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
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filepath, 
				 				aiProcess_ValidateDataStructure |
				 				aiProcess_Triangulate |
				 				aiProcess_FlipUVs |
								aiProcess_GenNormals |
				 				//aiProcess_GenSmoothNormals |
				 				aiProcess_OptimizeMeshes |
				 				aiProcess_OptimizeGraph |
								aiProcess_CalcTangentSpace);

			if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
			{
				std::cout << "Error: Assimp failed to load mesh from file. " << importer.GetErrorString() << std::endl;
				assert(false);
			}

			// Only take the first mesh from the file
			const aiMesh* p_aiMesh = scene->mMeshes[0];

			// Reorganize data into array of struct instead of separate arrays
			std::vector<vertex> vertices;
			std::vector<uint32_t> indices;
			vertex swap_vertex;
			for (size_t i = 0; i < p_aiMesh->mNumVertices; i++)
			{
				swap_vertex.position.x = p_aiMesh->mVertices[i].x;
				swap_vertex.position.y = p_aiMesh->mVertices[i].y;
				swap_vertex.position.z = p_aiMesh->mVertices[i].z;
				if (p_aiMesh->HasNormals())
				{
					swap_vertex.normal.x = p_aiMesh->mNormals[i].x;
					swap_vertex.normal.y = p_aiMesh->mNormals[i].y;
					swap_vertex.normal.z = p_aiMesh->mNormals[i].z;
				}
				if (p_aiMesh->HasTextureCoords(0))
				{
					swap_vertex.tex_coord.x = p_aiMesh->mTextureCoords[0][i].x;
					swap_vertex.tex_coord.y = p_aiMesh->mTextureCoords[0][i].y;
				}
				if (p_aiMesh->HasTangentsAndBitangents())
				{
					swap_vertex.tangent.x = p_aiMesh->mTangents[i].x;
					swap_vertex.tangent.y = p_aiMesh->mTangents[i].y;
					swap_vertex.tangent.z = p_aiMesh->mTangents[i].z;
					swap_vertex.bitangent.x = p_aiMesh->mBitangents[i].x;
					swap_vertex.bitangent.y = p_aiMesh->mBitangents[i].y;
					swap_vertex.bitangent.z = p_aiMesh->mBitangents[i].z;
				}
				vertices.push_back(swap_vertex);
			}
			for (size_t i = 0; i < p_aiMesh->mNumFaces; i++)
			{
				indices.emplace_back(p_aiMesh->mFaces[i].mIndices[0]);
				indices.emplace_back(p_aiMesh->mFaces[i].mIndices[1]);
				indices.emplace_back(p_aiMesh->mFaces[i].mIndices[2]);
			}
			r_mesh.num_indices = indices.size();

			auto fn = filepath + ".trace";
			std::ofstream myfile(fn);
			
			myfile << "vertices:" << std::endl;

			for (auto& v : vertices)
			{
				myfile << "position: ";					
				myfile << v.position.x << " " << v.position.y << " " << v.position.z;

				myfile << "; normal: ";
				myfile << v.normal.x << " " << v.normal.y << " " << v.normal.z << std::endl;
					
			}

			myfile << "indices:" << std::endl;

			for (int i = 0; i < indices.size(); i+=3)
			{				
				myfile << indices[i] << " " << indices[i+1] << " " << indices[i+2] << std::endl;
			}

			myfile.close();

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
		return r_mesh;
	}
}
