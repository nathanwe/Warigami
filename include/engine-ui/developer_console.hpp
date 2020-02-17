#ifndef __DEVCONSOLE_HPP_
#define __DEVCONSOLE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engineui
{

	class developer_console : public view
	{
		static const std::uint16_t BufSize = 4096;
		static const std::uint16_t InputBufSize = 2048;
		static const std::uint32_t Height = 256;

	public:
		developer_console(core::viewport& viewport, EventManager& events, GLFWwindow* window);
		void draw();

	private:
		// keep these static
		static char _outbuf[BUFSIZ];
		static char _errbuf[BUFSIZ];

		EventManager& _events;
		GLFWwindow* _window;

		char _command[BufSize]{ "" };
		char _output[BufSize]{ "" };
		char _error[BufSize]{ "" };
		char _input[InputBufSize]{ "" };

		void write_buffer(char* output, char* buffer);
	};
	
}

#endif
