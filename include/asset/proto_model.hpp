#ifndef WIZARD_ASSETS_PROTO_MODEL_HPP
#define WIZARD_ASSETS_PROTO_MODEL_HPP

#include "assimp/Importer.hpp"
#include "assimp/scene.h"

namespace asset
{
	struct proto_model
	{
		Assimp::Importer importer;
		const aiScene* aiscene;
	};
}

#endif
