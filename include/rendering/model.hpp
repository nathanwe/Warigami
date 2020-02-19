#ifndef WIZARD_RENDERING_MODEL_HPP
#define WIZARD_RENDERING_MODEL_HPP

#include "rendering/mesh_static.hpp"
#include "rendering/material_pbr.hpp"

#include <vector>

namespace rendering
{
	struct sub_model
	{
		mesh_static mesh;
		material_pbr material;
	};

	struct model
	{
		std::vector<sub_model> sub_models;
	};
}

#endif
