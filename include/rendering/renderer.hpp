#ifndef WIZARDPEOPLE_RENDERING_RENDERER_HPP
#define WIZARDPEOPLE_RENDERING_RENDERER_HPP

#include "ecs/state.hpp"
#include "ecs/system_base.hpp"
#include "rendering/render_pass.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace rendering
{
	class renderer : public ecs::system_base
	{
	public:
		renderer(GLFWwindow* window);

	public:
		virtual void update(ecs::state& state) override;

	private:
		GLFWwindow* _window;
		render_pass m_pass;
	};
}

#endif
