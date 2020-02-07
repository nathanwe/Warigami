#ifndef __DEVCONSOLE_HPP_
#define __DEVCONSOLE_HPP_

#include <functional>
#include <string>
#include <engine-ui/view.hpp>
#include <event/event_manager.hpp>

namespace engineui
{

	class developer_console : public view
	{
	public:
		developer_console(EventManager& events);

		void draw();

	private:
		EventManager& _events;

		char _output[4096]{ "" };
		char _input[2048]{ "" };
	};
	
}

#endif