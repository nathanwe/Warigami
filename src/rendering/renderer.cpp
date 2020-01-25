#include "rendering/renderer.hpp"

#include "ecs/state.hpp"

namespace rendering
{
	renderer::renderer(GLFWwindow* window) :
		_window(window),
		m_pass("default.vert", "default.frag")
	{
		
	}

	void renderer::update(ecs::state& state)
	{

	}
}
