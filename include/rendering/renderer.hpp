#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "rendering/render_pass.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <memory>

namespace rendering
{
	struct viewport
	{
		uint32_t x, y, width, height;
	};

	class renderer : public ecs::system_base
	{
	public:
		renderer(GLFWwindow* window, viewport window_view, bool is_debug);

	public:
		virtual void update(ecs::state& state) override;

	private:
		GLFWwindow* _window;
		viewport _window_view;
		std::unique_ptr<render_pass> _pass;
		bool _is_debug;
	};
}

#endif
