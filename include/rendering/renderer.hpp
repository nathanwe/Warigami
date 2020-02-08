#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "core/viewport.hpp"
#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "rendering/asset_cache.hpp"
#include "rendering/mesh_static.hpp"
#include "rendering/render_pass.hpp"
#include "rendering/render_state.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <memory>

namespace rendering
{
	

	class renderer : public ecs::system_base
	{
	public:
		renderer(GLFWwindow* window, core::viewport window_view, bool is_de, asset_cache& cache);

	public:
		virtual void update(ecs::state& state) override;

	private:
		bool _is_debug;
		GLFWwindow* _window;
		core::viewport _window_view;
		std::unique_ptr<render_pass> _pass_default;
		std::unique_ptr<render_pass> _pass_cubemap;
		mesh_static _mesh_cube;
		render_state _render_state;
	};
}

#endif
