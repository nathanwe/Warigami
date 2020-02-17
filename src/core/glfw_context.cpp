#include <core/glfw_context.hpp>


core::glfw_context::glfw_context(startup_config& conf) : _conf(conf)
{
	if (!glfwInit())
	{
		auto msg = "Error: GLFW failed to initialize.";
		std::cerr << msg << std::endl;
		throw std::runtime_error(msg);
	}

	uint32_t window_width = conf.width();
	uint32_t window_height = conf.height();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RED_BITS, 32);
	glfwWindowHint(GLFW_GREEN_BITS, 32);
	glfwWindowHint(GLFW_BLUE_BITS, 32);
	glfwWindowHint(GLFW_ALPHA_BITS, 32);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifndef NDEBUG	
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	_window = glfwCreateWindow(window_width, window_height, "Wizard People", nullptr, nullptr);
	if (!_window)
	{
		auto msg = "Error: GLFW failed to create window.";
		std::cerr << msg << std::endl;
		throw std::runtime_error(msg);
	}

	glfwMakeContextCurrent(_window);
	
	if (!conf.free_mouse())
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

core::glfw_context::~glfw_context()
{
	glfwTerminate();
}

GLFWwindow* core::glfw_context::window()
{
	return _window;
}

void core::glfw_context::swap_buffers()
{
	glfwSwapBuffers(_window);
}
