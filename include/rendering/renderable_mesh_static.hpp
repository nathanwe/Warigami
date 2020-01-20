#ifndef WIZARDPEOPLE_RENDERING_RENDERABLE_MESH_STATIC_HPP
#define WIZARDPEOPLE_RENDERING_RENDERABLE_MESH_STATIC_HPP

#include "ecs/component.hpp"
#include "rendering/component_bits.hpp"
#include "rendering/material_pbr.hpp"
#include "rendering/mesh_static.hpp"

namespace rendering
{
	struct renderable_mesh_static : ecs::component<renderable_mesh_static>
	{
		mesh_static mesh;
		material_pbr material;
	};
}

template<> const component_shift ecs::component<rendering::renderable_mesh_static>::component_bitshift = (component_shift)rendering::component_bits::renderable_mesh_static;

#endif
