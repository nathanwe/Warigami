#ifndef WIZARDPEOPLE_RENDERING_MESH_STATIC_HPP
#define WIZARDPEOPLE_RENDERING_MESH_STATIC_HPP

namespace rendering
{
	struct mesh_static
	{
		std::uint32_t vao = 0;
		std::uint32_t vbo = 0;
		std::uint32_t ebo = 0;
		std::uint32_t num_indices = 0;
	};
}

#endif
