#ifndef WIZARD_RENDERING_RENDERABLE_MODEL_STATIC_HPP
#define WIZARD_RENDERING_RENDERABLE_MODEL_STATIC_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"
#include "rendering/material_pbr.hpp"
#include "rendering/mesh_static.hpp"
#include "rendering/model.hpp"

namespace rendering
{
	struct renderable_model_static : ecs::component<renderable_model_static>
	{
		model model_;
	};
}

#endif
