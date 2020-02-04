#ifndef __CONSOLE_EUI_VIEW_HPP_
#define __CONSOLE_EUI_VIEW_HPP_

#include <functional>

#include <engine-ui/models/console.hpp>

namespace engineui::view
{

	class console
	{
	public:
		console(engineui::model::console& model) : _model(model) {}

		void on_command(std::function<void(std::string)> callback);
		void render();

	private:
		engineui::model::console& _model;
		std::function<void(std::string)> _command_callback;
	};

}

#endif
