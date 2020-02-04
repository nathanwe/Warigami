#ifndef __CONSOLE_EUI_MODEL_HPP_
#define __CONSOLE_EUI_MODEL_HPP_

#include <string>

namespace engineui::model
{
	struct console
	{
		const char output[4096];
		const char input[1024];		
	};
}

#endif