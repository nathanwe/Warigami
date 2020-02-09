#ifndef __DEVCONSOLE_HPP_
#define __DEVCONSOLE_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>

namespace engineui
{

	class developer_console : public view
	{
		static const std::uint16_t BufSize = 4096;
		const float Height = 256.f;


	public:
		developer_console(core::viewport& viewport, EventManager& events);
		void draw();

	private:
		EventManager& _events;

		static char _outbuf[BUFSIZ];
		static char _errbuf[BUFSIZ];

		char _command[BufSize]{ "" };
		char _output[BufSize]{ "" };
		char _error[BufSize]{ "" };

		char _input[2048]{ "" };

		void write_buffer(char* output, char* buffer);
	};
	
}

#endif
