#ifndef WIZARD_ASSETS_PROTO_MESH_HPP
#define WIZARD_ASSETS_PROTO_MESH_HPP

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace asset
{
	struct proto_mesh
	{
		Assimp::Importer importer;
		aiMesh* assimp_mesh;
		const aiScene* assimp_scene;
	};
}

#endif
