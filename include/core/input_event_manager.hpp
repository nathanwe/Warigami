#ifndef __INPUT_EVENT_MANAGER_HPP_
#define __INPUT_EVENT_MANAGER_HPP_

namespace core {

	enum input_events {
		UP_EVENT,
		DOWN_EVENT,
		LEFT_EVENT,
		RIGHT_EVENT,
		MENU_EVENT,
		CONFIRM_EVENT,
		BACK_EVENT,
		ACTION1_EVENT,
		ACTION2_EVENT,
		ACTION3_EVENT
	};

	class input_event_manager {
	public:
		bool is_input_active(input_events input_event);
		bool is_input_started(input_events input_event);
		bool is_input_ended(input_events input_event);

		void update();
	private:
		static const int _event_count = 1;
		bool _prev_game_state[_event_count];
		bool _current_game_state[_event_count];
	};
}

#endif //__INPUT_EVENT_MANAGER_HPP_