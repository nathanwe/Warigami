#ifndef __INPUT_EVENT_MANAGER_HPP_
#define __INPUT_EVENT_MANAGER_HPP_

#include "core/input_manager.hpp"

namespace core {

	enum controls {
		UP_CONTROL,
		DOWN_CONTROL,
		LEFT_CONTROL,
		RIGHT_CONTROL,
		CARD1_CONTROL,
		CARD2_CONTROL,
		CARD3_CONTROL,
		CARD4_CONTROL,
		DIE1_CONTROL,
		DIE2_CONTROL,
		PULL_AXIS,

		UP_CONTROL_PLAYER2,
		DOWN_CONTROL_PLAYER2,
		LEFT_CONTROL_PLAYER2,
		RIGHT_CONTROL_PLAYER2,
		CARD1_CONTROL_PLAYER2,
		CARD2_CONTROL_PLAYER2,
		CARD3_CONTROL_PLAYER2,
		CARD4_CONTROL_PLAYER2,
		DIE1_CONTROL_PLAYER2,
		DIE2_CONTROL_PLAYER2,
		PULL_AXIS_PLAYER2,

		MENU_CONTROL,
		CONFIRM_CONTROL,
		BACK_CONTROL,
		DEVELOPER_CONSOLE,
		
		START,
		BACK,
		
		CONTROL_COUNT		
	};

	// -1 to 1
	typedef float input_axis;

	typedef float view_axis;

	class game_input_manager {
	public:
		game_input_manager(input_manager input);
		bool is_input_active(controls control);
		bool is_input_started(controls control);
		bool is_input_ended(controls control);

		void update();

		input_axis forward() const { return _forward; }
		input_axis strafe() const { return _strafe; }
		input_axis right_trigger() const { return _right_trigger; }
		view_axis pitch() const { return _pitch; }
		view_axis yaw() const { return _yaw; }

		input_axis forward_player2() const { return _forward_player2; }
		input_axis strafe_player2() const { return _strafe_player2; }		
		input_axis right_trigger2() const { return _right_trigger_player2; }
		view_axis pitch_player2() const { return _pitch_player2; }
		view_axis yaw_player2() const { return _yaw_player2; }

		bool any_button_pressed() const { return _any_button_pressed; }

	private:
		input_manager _input;
		bool _prev_game_state[CONTROL_COUNT];
		bool _current_game_state[CONTROL_COUNT];
		bool _any_button_pressed;

		input_axis _forward{ 0 };
		input_axis _strafe{ 0 };
		view_axis _pitch{ 0 };
		view_axis _yaw{ 0 };

		input_axis _right_trigger{ 0 };

		input_axis _forward_player2{ 0 };
		input_axis _strafe_player2{ 0 };
		view_axis _pitch_player2{ 0 };
		view_axis _yaw_player2{ 0 };
		input_axis _right_trigger_player2{ 0 };
	};
}

#endif //__INPUT_EVENT_MANAGER_HPP_