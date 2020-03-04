#ifndef __GLFW_CONTEXT_HPP_
#define __GLFW_CONTEXT_HPP_

#include <iostream>
#include <cstdint>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <core/startup_config.hpp>

namespace core
{

	class glfw_context
	{
	public:
		glfw_context(startup_config& conf);
		~glfw_context();

		GLFWwindow* window();
		void swap_buffers();

		std::uint32_t width() const;
		std::uint32_t height() const;

	private:
		GLFWwindow* _window;
		startup_config& _conf;

		std::uint32_t _width;
		std::uint32_t _height;
	};

}
#endif