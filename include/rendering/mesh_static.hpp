#ifndef WIZARDPEOPLE_RENDERING_MESH_STATIC_HPP
#define WIZARDPEOPLE_RENDERING_MESH_STATIC_HPP

namespace rendering
{
	struct mesh_static
	{
		std::uint32_t vao;
		std::uint32_t vbo;
		std::uint32_t ebo;
		std::uint32_t num_indices;
	};
}

#endif
