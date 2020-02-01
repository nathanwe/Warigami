#ifndef __INPUT_EVENT_MANAGER_HPP_
#define __INPUT_EVENT_MANAGER_HPP_

#include "core/input_manager.hpp"

namespace core {

	enum controls {
		UP_CONTROL,
		DOWN_CONTROL,
		LEFT_CONTROL,
		RIGHT_CONTROL,
		MENU_CONTROL,
		CONFIRM_CONTROL,
		BACK_CONTROL,
		ACTION1_CONTROL,
		ACTION2_CONTROL,
		ACTION3_CONTROL,
		ACTION4_CONTROL,
		DESCEND_CONTROL,
		ASCEND_CONTROL,
		INTERACT_CONTROL,
		CONTROL_COUNT
	};

	class game_input_manager {
	public:
		game_input_manager(input_manager input);
		bool is_input_active(controls control);
		bool is_input_started(controls control);
		bool is_input_ended(controls control);

		void update();
	private:
		input_manager _input;
		bool _prev_game_state[CONTROL_COUNT];
		bool _current_game_state[CONTROL_COUNT];
	};
}

#endif //__INPUT_EVENT_MANAGER_HPP_