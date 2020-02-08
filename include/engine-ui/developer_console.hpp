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
		const float Height = 200.f;

	public:
		developer_console(core::viewport& viewport, EventManager& events);
		void draw();

	private:
		EventManager& _events;

		char _output[4096]{ "" };
		char _input[2048]{ "" };
	};
	
}

#endif
